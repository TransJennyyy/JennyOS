[org 0x000]
[bits 32]
jmp short AppEntry ; make sure its an OFFSET and not a memory address
times 20-($-$$) db 0
; HERE EDI stores offset

AppEntry:


    mov esi, 0 ; print syscall
    mov ebx, HelloWorldPrint
    add ebx, edi
    int 0x80

    mov esi, 0x10
    mov ebx, CommandLinePath
    add ebx, edi
    xor edx, edx
    int 0x80

    .InfLoop:
        
        inc byte [0xb8020]

        jmp short .InfLoop


CommandLinePath db '/CommandLine.jexe',0

HelloWorldPrint db 'Hello World From Test App', 0x0A, 0