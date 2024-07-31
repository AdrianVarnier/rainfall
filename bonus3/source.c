//----- (080484F4) --------------------------------------------------------
int __cdecl main(int argc, const char **argv, const char **envp)
{
  char ptr[132]; // [esp+18h] [ebp-90h] BYREF
  FILE *v5; // [esp+9Ch] [ebp-Ch]

  v5 = fopen("/home/user/end/.pass", "r");
  memset(ptr, 0, sizeof(ptr));
  if ( !v5 || argc != 2 )
    return -1;
  fread(ptr, 1u, 0x42u, v5);
  ptr[65] = 0;
  ptr[atoi(argv[1])] = 0;
  fread(&ptr[66], 1u, 0x41u, v5);
  fclose(v5);
  if ( !strcmp(ptr, argv[1]) )
    execl("/bin/sh", "sh", 0);
  else
    puts(&ptr[66]);
  return 0;
}