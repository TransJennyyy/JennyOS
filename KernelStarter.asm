[bits 16]
[org 0x7e00]

%define ATAMainBus 0x1F0

jmp __Entry


BIOSDiskNumber db 0

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

gdt_end32:


gdtinfo:
   dw gdt_end - gdt - 1   ;last byte in table
   dd gdt                 ;start of table

gdt:        dd 0,0        ; entry 0 is always unused
codedesc:   db 0xff, 0xff, 0, 0, 0, 10011010b, 00000000b, 0
flatdesc:   db 0xff, 0xff, 0, 0, 0, 10010010b, 11001111b, 0
gdt_end:

DiskReadErrors:
    mov eax, 0xb8006
    mov [ds:eax], byte 'E'
    hlt


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
    
    mov [DiskReadData.LBANumber], dword ecx

    mov ah, 0x42
    mov si, DiskReadData
    mov dl, [BIOSDiskNumber]
    int 0x13
    jc DiskReadErrors


    
    popa
    ret

CurrentMemoryOffset dd 0x180000

CopySec:
    pusha
    
    mov eax, [CurrentMemoryOffset]
    mov si, 0x8000
    mov cx, 512
    .WriteLoop:

        mov dl, [si]
        mov [ds:eax], byte dl
        inc si
        inc eax

        dec cx
        jnz .WriteLoop

    mov [CurrentMemoryOffset], dword eax


    popa
    ret



__Entry:

    in al, 0x92
    or al, 2
    out 0x92, al

    xor ax, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov sp, 0x1000
    mov bp, 0x1000
    cli
    push ds

    lgdt [gdtinfo]

    mov eax, cr0
    or eax, 1
    mov cr0, eax

    jmp 08h:.ProtectedMode

    .ProtectedMode:
        mov bx, 0x10
        mov ds, bx

        and al, 0xFE
        mov cr0, eax
        jmp 0x0:.UnrealMode
    .UnrealMode:
        pop ds
        sti


        mov [BIOSDiskNumber], byte dl
        mov ecx, 2
        .ReadLoop:

            push ecx
            call DiskRead
            pop ecx

            push ecx
            call CopySec
            pop ecx


            inc ecx
            cmp ecx, 258
            jne .ReadLoop

        
        cli
        lgdt [gdt_ptr]
        mov eax, cr0
        or eax, 1
        mov cr0, eax
        jmp 0x08:ProtectedEntry
        




[bits 32]

KernelCheckFailed:
    mov [0xb8000], byte 'E'
    mov [0xb8002], byte 'R'
    hlt

ProtectedEntry:
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov esp, 0x10000
    mov ebp, esp


    cmp [0x180000], byte 'J'
    jne KernelCheckFailed
    cmp [0x180001], byte 'O'
    jne KernelCheckFailed
    cmp [0x180002], byte 'S'
    jne KernelCheckFailed
    cmp [0x180003], byte 'K'
    jne KernelCheckFailed


    mov eax, 1
    mov cr0, eax
    dec eax
    mov cr3, eax
    mov cr4, eax
    mov dl, [BIOSDiskNumber]
    jmp 0x180004
        



times 512-($-$$) db 0