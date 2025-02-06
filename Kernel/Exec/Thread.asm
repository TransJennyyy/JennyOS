


global StartThread
StartThread:
    mov eax, [esp+4] ; App Entry
    mov ecx, [esp+8] ; app Stack PTR
    pop ebx ; get rid of the ret
    mov ebx, eax

    

    mov esp, ecx
    mov ebp, esp
    sti
    mov edi, ebx
    jmp ebx


global ReStartThread
ReStartThread:
    mov eax, [esp+4] ; App Current Stack PTR
    pop ebx

    mov esp, eax
    popad
    popf
    sti
    iret