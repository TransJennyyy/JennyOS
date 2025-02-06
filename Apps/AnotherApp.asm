[org 0x000]
[bits 32]
db 'Hello world2'
times 20-($-$$) db 0
; HERE EDI stores offset

jmp short AppEntry
TestPrintStr db 'Hello World From Another App', 0x0A, 0
AppEntry:
    
    mov [0xb8042], byte 'A'

    mov esi, 0
    mov ebx, TestPrintStr
    int 0x80

    .Update:
        inc  byte [0x8042]
    jmp short .Update



