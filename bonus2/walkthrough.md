bonus2 protections:
```Shell
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/bonus2/bonus2
```

bonus2 rights:
```Shell
bonus2@RainFall:~$ ls -la bonus2
-rwsr-s---+ 1 bonus3 users 5664 Mar  6  2016 bonus2
```