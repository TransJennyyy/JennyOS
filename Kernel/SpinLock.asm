global SpinLockReset

SpinLockReset:
    mov eax, [esp+4]
    mov esi, [esp+8]


    xchg eax, [esi]
    ret
