bonus1 protections:
```Shell
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/bonus1/bonus1
```

bonus1 rights:
```Shell
bonus1@RainFall:~$ ls -la bonus1 
-rwsr-s---+ 1 bonus2 users 5043 Mar  6  2016 bonus1
```