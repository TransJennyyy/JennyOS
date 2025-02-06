[org 0x7c00]
[bits 16]
jmp .MainCode
times 0x24-($-$$) db 0

.MainCode:
xor ah, ah
mov al, 3
int 0x10

mov ah, 0x0e
mov al, 'S'
int 0x10

mov eax, 0
mov cr0, eax
mov cr2, eax
mov cr3, eax
mov cr4, eax


jmp _BootLoaderEntry
Error:
    mov ah, 0x0e
    mov al, 'E'
    int 0x10
    jmp $

_BootLoaderEntry:
    

    mov ah, 0x02
    mov al, 1
    xor cx, cx
    mov cl, 2
    xor dh, dh
    xor bx, bx
    mov es,bx
    mov bx, 0x7e00
    int 0x13

    jc Error


    jmp 0x7e00



    

times 446-($-$$) db 0

db 00000010b
dw 0 ; CHS (not using)
db 0 
db 0xEE ; parition type (marked as bootable so other os's know)
dw 0 ; CHS end
db 0
dd 0 ; LBA start
dd 258 ; number sectors (this is the bootloader + kernel size)

db 00000010b
dw 0 ; CHS (not using)
db 0 
db 0x7f ; parition type (marked as Jenny os File System)
dw 0 ; CHS end
db 0
dd 258 ; LBA start
dd ((8*1024*1024*1024)/512) ; The Filesystems defualt size is 8GB

db 00000010b
dw 0 ; CHS (not using)
db 0 
db 0x7f ; parition type (marked as Jenny os File System)
dw 0 ; CHS end
db 0
dd (((8*1024*1024*1024)/512) +258) ; LBA start
dd (2*1024*1024)/512 ; This Partision is ment for a backup of usefull os files. it will be 2 mib in size



times 510-($-$$) db 0

dw 0xaa55
