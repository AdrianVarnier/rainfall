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

When testing the binary, it didn't do any particular thing. So let's decompile it with Hex-Rays on https://dogbolt.org/ and see what is there:
- without argument, the program exits
- it seems to use a class or function `N`
- there is a call to a `setAnnotation()` function
- unlike the other levels, there is no `system()` calls
- the `main()` returns the result of a complicated function call that dereferences things

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
The segfault happens at 0x08048682 <+142>, when the content pointed by the address in `EAX` is moved into `EDX`.

```Shell
(gdb) info register eax
eax            0x41366441       1094083649
```



0xb7d86060

\x60\x60\xd8\xb7 (system)

0x804a00c (start of the buffer in $eax)

</p>