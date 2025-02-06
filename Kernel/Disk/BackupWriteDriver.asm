


global DiskWriteFunc
DiskWriteFunc:
    mov eax, [esp+4]
    pusha
    pushf
    cli

    mov [(StartStackW-DiskWriteFunc)+0x7e00], dword esp

    mov [((DiskWriteInfo.LBAWrite-DiskWriteFunc) +0x7e00)], dword eax

    mov eax, cr0
    mov [(StartCr0W-DiskWriteFunc)+0x7e00], dword eax

    mov eax, cr3
    mov [(StartCr3W-DiskWriteFunc)+0x7e00], dword eax

    jmp 0x18:((.RealModeEntry-DiskWriteFunc)+0x7e00)

    [bits 16]
    .RealModeEntry:
        mov ax, 0x20
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov ss, ax
        mov gs, ax

        mov eax, cr0
        and eax, 0x7FFFFFFe
        mov cr0, eax

        jmp 0:((.RealModeCode-DiskWriteFunc)+0x7e00)

    .RealModeCode:	
        mov sp, 0x1000
        xor ax, ax
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov ss, ax
        mov gs, ax

        lidt [(RealModeIDT-DiskWriteFunc)+0x7e00]
        sti


        mov ah, 0x43
        xor al, al
        mov dl, [(DriveLetter-DiskWriteFunc)+0x7e00]
        mov si, ((DiskWriteInfo-DiskWriteFunc)+0x7e00)
        int 0x13

        cli

        lgdt [(gdt_ptr-DiskWriteFunc)+0x7e00]
        mov eax, cr0
        or eax, 1
        mov cr0, eax

        jmp 08h:((ProTectedModeEntry-DiskWriteFunc)+0x7e00)
    



    

DiskWriteInfo:
    db 0x10
    db 0
    dw 1
    dd 0x8000
    .LBAWrite: dq 0 ; we change this 


RealModeIDT:
    dw 0x3ff
    dd 0



StartCr0W dd 0
StartCr3W dd 0
StartStackW dd 0

WriteLBA dd 0

gdt_ptr:
    dw gdt_end32 - gdt_start - 1  ; Limit (Size of GDT - 1)
    dd gdt_start                ; Base address of GDT

gdt_start:
    ; Null descriptor
    dq 0x0000000000000000

    ; Code segment descriptor
    dw 0xFFFF            ; Limit (low 16 bits)
    dw 0x0000            ; Base (low 16 bits)
    db 0x00              ; Base (middle 8 bits)
    db 10011010b         ; Access byte: present, ring 0, code segment, execute/read
    db 11001110b         ; Granularity byte: 4K granularity, 32-bit protected mode, Limit (high 4 bits) = 0
    db 0x00              ; Base (high 8 bits)

    ; Data segment descriptor
    dw 0xFFFF            ; Limit (low 16 bits)
    dw 0x0000            ; Base (low 16 bits)
    db 0x00              ; Base (middle 8 bits)
    db 10010010b         ; Access byte: present, ring 0, data segment, read/write
    db 11001110b         ; Granularity byte: 4K granularity, 32-bit protected mode, Limit (high 4 bits) = 0
    db 0x00              ; Base (high 8 bits)

    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10011010b
    db 00010000b
    db 0x00

    dw 0xFFFF
    dw 0x0000
    db 0x0
    db 10010010b
    db 00010000b
    db 0x00
gdt_end32:

DriveLetter db 0

[bits 32]
extern IdtrPTR
ProTectedModeEntry:
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov esp, [(StartStackW-DiskWriteFunc)+0x7e00]

    mov eax, [(StartCr3W-DiskWriteFunc)+0x7e00]
    mov cr3, eax

    mov eax, [(StartCr0W-DiskWriteFunc)+0x7e00]
    mov cr0, eax

    mov eax, [IdtrPTR]
    lidt [eax]
    popf
    popa
    ret


WriteFuncEnd:

DiskWriteFuncSize equ (WriteFuncEnd-DiskWriteFunc)

extern BIOSDiskNumber
global CopyWriteFunc
CopyWriteFunc:
    pusha
    mov al, [BIOSDiskNumber]
    mov [DriveLetter], byte al

    xor ecx, ecx
    mov esi, DiskWriteFunc
    .CopyLoop:
        mov al, [esi+ecx]
        mov [0x7e00+ecx], byte al
        inc ecx
        cmp ecx, DiskWriteFuncSize
        jle .CopyLoop
    popa
    ret
        