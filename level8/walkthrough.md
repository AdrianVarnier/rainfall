level8 protections:
```Shell
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/level8/level8
```

level8 rights:
```Shell
level8@RainFall:~$ ls -la level8 
-rwsr-s---+ 1 level9 users 6057 Mar  6  2016 level8
```

observations:
Decompiling with Hex-Rays on https://dogbolt.org/ shows four things:
- `fgets` takes keywords that have different purposes.
- the keyword `auth` initialises the `auth` variable with a `malloc(8)`.
- the keyword `service` initialises the `service` variable with a `malloc(8)`.
- the keyword `login` opens a shell if login + 32 is different from 0