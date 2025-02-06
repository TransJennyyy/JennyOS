
section .text
extern IdtrPTR
extern BIOSDiskNumber




global DiskReadFunc
DiskReadFunc:
    cli
    mov eax, [esp+4]
    pusha
    pushf

    ;mov [0xb8000], byte 'S'

    mov [((Sector-DiskReadFunc) + 0x7e00)], dword eax

    jmp 0x18:((.RealModeJump-DiskReadFunc) + 0x7e00)

    [bits 16]

    .RealModeJump:
        mov ax, 0x20
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax

        ;mov eax, 0xb8000
        ;mov [eax], byte 'R'
    

        mov [(StartStack-DiskReadFunc)+0x7e00], dword esp

        mov eax, cr3
        mov [((StartCr3-DiskReadFunc)+0x7e00)], dword eax

        mov eax, cr0
        mov [((StartCr0-DiskReadFunc)+0x7e00)], dword eax
        and eax, 0x7FFFFFFe	
        mov cr0, eax
        jmp 0:((RealModeFinnal-DiskReadFunc) + 0x7e00)

StartCr0 dd 0
StartCr3 dd 0
StartStack dd 0
    

WantedOutputAddress dd 0
NumberSectorsWanted dd 0
Sector dd 0

RealModeIDT:
    dw 0x3ff
    dd 0

RealModeFinnal:
    mov sp, 0x1000 ; this area is resurved for real mode apps
    xor ax, ax
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    lidt [((RealModeIDT-DiskReadFunc) + 0x7e00)]
    sti

    ;mov eax, 0xb8000
    ;mov [eax], byte 'F'

    mov ecx, [((Sector-DiskReadFunc) + 0x7e00)]
    mov eax, ((DiskRead-DiskReadFunc) + 0x7e00)
    call eax

    cli
    lgdt [((gdt_ptr-DiskReadFunc)+0x7e00)]
    mov eax, [((StartCr0-DiskReadFunc)+0x7e00)]
    mov cr0, eax


    jmp 08h:((ProtectedReEntry-DiskReadFunc)+0x7e00)
    

    
    
DiskReadRetCode dd 0
BIOSDISKNum db 0
DiskReadData:
    db 0x10
    db 0
    dw 1
    dw 0x8000
    dw 0
    .LBANumber dd 0 ; to change dynaimicly
    dd 0 


DiskRead: ; ecx(sector number)
    pusha
    
    mov [((DiskReadData.LBANumber-DiskReadFunc) + 0x7e00)], dword ecx

    
    ;mov eax, 0xb8000
    ;mov [eax], byte 'A'

    mov ah, 0x42
    mov si, ((DiskReadData-DiskReadFunc) + 0x7e00)
    mov dl, [((BIOSDISKNum-DiskReadFunc) + 0x7e00)]
    int 0x13

    ;mov eax, 0xb8000
    ;mov [eax], byte 'B'

    popa
    ret

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







[bits 32]

ProtectedReEntry:
        mov ax, 0x10
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax


        jmp DiskReadRet

DiskReadRetAddr dw ((DiskReadRet-DiskReadFunc)+0x7e00)
DiskReadRet:

    mov esp, [(StartStack-DiskReadFunc)+0x7e00]
    popf
    popa

    mov eax, [(StartCr3-DiskReadFunc)+0x7e00]
    mov cr3, eax

    mov eax, [IdtrPTR]
    lidt [eax]
    cli

    ret


DiskReadFuncEnd:

DiskReadFuncSize equ DiskReadFuncEnd - DiskReadFunc










global BackUpSystemInit
BackUpSystemInit:
    pusha
    mov al, [BIOSDiskNumber]
    mov [BIOSDISKNum], byte al
    lgdt [gdt_ptr]

    mov ecx, DiskReadFuncSize

    mov esi, 0x7e00
    mov edi, DiskReadFunc
    .CopyLoop:

        mov al, [edi]
        mov [esi], al
        inc edi
        inc esi

        dec ecx
        jnz .CopyLoop
    popa
    ret

