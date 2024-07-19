int __cdecl main(int argc, const char **argv, const char **envp)
{
  N *v3; // ebx
  N *v4; // ebx
  N *v6; // [esp+1Ch] [ebp-8h]

  if ( argc <= 1 )
    _exit(1);
  v3 = (N *)operator new(0x6Cu);
  N::N(v3, 5);
  v6 = v3;
  v4 = (N *)operator new(0x6Cu);
  N::N(v4, 6);
  N::setAnnotation(v6, (char *)argv[1]);
  return (**(int (__cdecl ***)(N *, N *))v4)(v4, v6);
}

//----- (080486F6) --------------------------------------------------------
void __cdecl N::N(N *this, int a2)
{
  *(_DWORD *)this = off_8048848;
  *((_DWORD *)this + 26) = a2;
}
// 8048848: using guessed type int (__cdecl *off_8048848[2])(int, int);

//----- (0804870E) --------------------------------------------------------
void *__cdecl N::setAnnotation(N *this, char *s)
{
  size_t v2; // eax

  v2 = strlen(s);
  return memcpy((char *)this + 4, s, v2);
}