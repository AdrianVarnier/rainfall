INFO LVL
INFO EXE RIGHT

OBSERVATION
Using a disassembler we see a buffer of `500 bytes` in `n` function used by `fgets` and `printf`.
`fgets` can't be exploit to do a buffer overflow.
`printf` only use our buffer as argument, when it receive only a format string it will use stack address as argument.
The vulnerability comes from the use of `printf` with the buffer content without sanitize it, allowing us to print value from the stack with `%p` and write on it using `%n`.
We also see that a global variable `m` is compared with `64` and in case of equality open a shell.

STRATEGY

%n write in the address specified as argument the value corresponding to the number of byte printf has print before its occurence.
We need to find `m` address to overwrite it with the value 64 and access a shell.
We need to find the position of our buffer on the stack when using printf because we will use it as argument of %n with the address of m as value.

I. FINDING M ADDRESS
Using gdb we can see m address stored in eax and compared with 64.

```Shell
(gdb) disas v
[...]
 0x080484da <+54>:    mov    0x804988c,%eax
 0x080484df <+59>:    cmp    $0x40,%eax
[...]

 ```

II. FINDING BUFFER POSITION
Using this format string `AAAA %p %p %p %p` we will print the content of the four stack address that printf will use as argument and observe that the fourth is our buffer, because it's content is fill of 0x41 which is 'A' value in ASCII, the first four character of our buffer.

```Shell
level3@RainFall:~$ ./level3
AAAA %p %p %p %p
AAAA 0x200 0xb7fd1ac0 0xb7ff37d0 0x41414141

```

III. OVERWRITING M
We will use `%[argument position]$[specifier]` syntax to give our specifier a chosen argument in the format string.
We will use the 4th argument which corespond to our buffer where we will store m address, so m will be the destination of the write that %n will perform.
We also need to print 64 bytes with printf before using %n, an address is 4 bytes so we need to add 60 characters to overwrite m value with 64.
So our payload will be:

```Shell
python -c 'print("\x8c\x98\x04\x08" + "A" * 60 + "%4$n")' > /tmp/input3

```

cat /tmp/input3 - | ./level3