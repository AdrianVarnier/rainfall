Bonus1 protections:
```Shell
RELRO           STACK CANARY      NX            PIE             RPATH      RUNPATH      FILE
No RELRO        No canary found   NX disabled   No PIE          No RPATH   No RUNPATH   /home/user/bonus1/bonus1
```

Bonus1 rights:
```Shell
bonus1@RainFall:~$ ls -la bonus1 
-rwsr-s---+ 1 bonus2 users 5043 Mar  6  2016 bonus1
```

Let's decompile it with Hex-Rays on https://dogbolt.org/ and see what is there:
- there is only a `main()` function and it takes two arguments
- `atoi()` is used on the first argument and stores the result an integer called `v5`
- a condition prevents `v5` from being greater than 9, exiting the program otherwise
- then, `v5` is compared to 1464814662, and if equal, it opens a shell
- a call to `memcpy()` is made using a 40-bytes buffer as dest and the second argument as src, with a size of 4 times the value of `v5`

We know that local variables follow each other in the stack frame. If we can overflow the buffer, we could change the value of `v5` to 1464814662, which is 574F4C46 in hexadecimal.

However, the first argument needs to be equal to 11 if we want to do an overflow (11 * 4 = 44, which is the size of our payload if we want to reach `v5`). But how do we bypass the condition that it shouldn't be greater than 9? 
`int` can be negative but `memcpy()` takes a `size_t` (unsigned int) as parameter. When converted into `size_t`, a negative value can become positive again because of the underflow: if we 
pass the right negative value, we could therefore obtain something that would be equal to 11.

Let's see:
`SIZE_MAX`, the maximum for a `size_t` on a 32-bits system, is 2147483647
`INT_MIN`, the minimum for an integer, is -2147483648.  

Doing `-(SIZE_MAX)` underflows to 0, so if we want `v5` to be 11, we need to send `-(SIZE_MAX - 11)` as our first parameter.  This gives us -2147483636 which is greater than `INT_MIN`.
Our second argument will be 40 bytes of padding followed by 1464814662 converted in Little Endian.

And the result is:
```Shell
bonus1@RainFall:~$ ./bonus1 -2147483636 $(python -c 'print("A" * 40 + "\x57\x4f\x4c\x46"[::-1])')
$ cat /home/user/bonus2/.pass
579bd19263eb8655e4cf7b742d75edf8c38226925d78db8163506f5191825245
```