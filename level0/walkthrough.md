<p align="justify"> 

We will get information about various security features that are disabled at each level. This allows us to research
the best method to exploit binaries.
```Shell
level0@localhost's password: 
  GCC stack protector support:            Enabled
  Strict user copy checks:                Disabled
  Restrict /dev/mem access:               Enabled
  Restrict /dev/kmem access:              Enabled
  grsecurity / PaX: No GRKERNSEC
  Kernel Heap Hardening: No KERNHEAP
 System-wide ASLR (kernel.randomize_va_space): Off (Setting: 0)
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX enabled    No PIE          No RPATH   No RUNPATH   /home/user/level0/level0
```

We also notice that the binary has level1 rights:
```Shell
-rwsr-x---+ 1 level1 users  747441 Mar  6  2016 level0
```

Here is what we get when testing:
```Shell
level0@RainFall:~$ ./level0
Segmentation fault (core dumped)
level0@RainFall:~$ ./level0 ieiusehf
level0@RainFall:~$ ./level0 ieiusehf
No !
```

Decompiling with Snowman on https://dogbolt.org/ shows two things:
- `atoi()` is used to compare the first argument passed to the program with 0x1a7 (423)
- if the result is positive, it opens a shell that allows us to access the password.
```Shell
level0@RainFall:~$ ./level0 423

$ cat /home/user/level1/.pass
1fe8a524fa4bec01ca4ea2a869af2a02260d4a7d5fe7e7c24d8617e6dca12d3a
```

</p>