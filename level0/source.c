int32_t main(void** ecx) {
    void** v2;
    struct s1584* v3;
    void** eax4;
    void** eax5;
    void** v6;
    void** v7;
    void** v8;
    void** v9;
    void** v10;
    void** v11;
    void** v12;
    void** eax13;
    void** eax14;
    void** eax15;
    void** v16;
    void** v17;
    void** v18;
    void** v19;
    void** v20;
    void** v21;
    void** v22;
    void** v23;
    void** v24;
    void** v25;

    v2 = v3->f4;
    eax4 = atoi(v2);
    if (!reinterpret_cast<int1_t>(eax4 == 0x1a7)) {
        eax5 = _IO_stderr;
        fwrite(ecx, "No !\n", 1, 5, eax5);
    } else {
        eax13 = strdup(ecx, "/bin/sh", v6, v7, v8, v9, v10, v11, v12);
        eax14 = __getegid("/bin/sh");
        eax15 = __geteuid("/bin/sh");
        setresgid(eax14, eax14, eax14, v16, eax13, 0, eax15, eax14, v17, v18, v19, v20);
        __setresuid(eax15, eax15, eax15, v21, eax13, 0, eax15, eax14, v22, v23, v24, v25);
        execv("/bin/sh", (reinterpret_cast<uint32_t>(reinterpret_cast<int32_t>(__zero_stack_offset()) - 4) & 0xfffffff0) - 32 - 4 + 4 - 4 + 4 - 4 + 4 - 4 + 4 - 4 + 4 - 4 + 4 + 16, eax15);
    }
    return 0;
}
