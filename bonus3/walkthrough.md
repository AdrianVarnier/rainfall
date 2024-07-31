Bonus3 protections:
```Shell
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX enabled    No PIE          No RPATH   No RUNPATH   /home/user/bonus3/bonus3
```

Bonus3 rights:
```Shell
-rwsr-s---+ 1 end    users  5595 Mar  6  2016 bonus3
```

Testing:
```Shell
bonus3@RainFall:~$ ./bonus3
bonus3@RainFall:~$ ./bonus3 ourosrghosug

```

Let's decompile it with Hex-Rays on https://dogbolt.org/ and see what is there:
- it takes an argument and uses `atoi()` to retrieve its value
- it makes a comparison between the argument and a null pointer, opening a shell if the two strings are the same
- we also learn that the next user is not called bonusX but `end`

We tried a null string and here it is:
```Shell
bonus3@RainFall:~$ ./bonus3 ""
$ cat /home/user/end/.pass
3321b6f81659f9a71c76616f606e4b50189cecfea611393d5d649f75e157353c
```