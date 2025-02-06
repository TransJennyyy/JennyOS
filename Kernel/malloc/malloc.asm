

global MallocMemoryMapEntry
MallocMemoryMapEntry:
    pushf
    pushad
    mov [StartStack], esp
    jmp 0x18:((.RealModeEntry-MallocMemoryMapEntry)+0x7e00)
    [bits 16]
    .RealModeEntry:
        mov ax, 0x20
        mov ds, ax
        mov es, ax
        mov ss, ax
        mov fs, ax
        mov gs, ax


        

        mov eax, cr3
        mov [((Cr3Store-MallocMemoryMapEntry)+0x7e00)], dword eax

        mov eax, cr0
        mov [((Cr0Store-MallocMemoryMapEntry)+0x7e00)], dword eax
        and eax, 0x7FFFFFFe	
        mov cr0, eax

        jmp 0:((.FinnalRealMode-MallocMemoryMapEntry)+0x7e00)

    .FinnalRealMode:
        mov ax, 0
        mov ds, ax
        mov es, ax
        mov ss, ax
        mov gs, ax
        mov fs, ax

        mov sp, 0x1000

        lidt [(LidtRealMode-MallocMemoryMapEntry+0x7e00)]
        sti
        xor bp, bp
        xor edi, edi
        mov di, 0x8004
        push edi
        mov [es:di + 20], dword 1 ; ask for ACPI v3 
        mov eax, 0xE820 
        xor ebx, ebx
        mov edx, 0x534D4150
        mov ecx, 24
        int 0x15
        jc short .ErrorMemoryMap
        pop edi
        mov edx, 0x534D4150
        cmp eax, edx
        jne short .ErrorMemoryMap

        test ebx, ebx
        je short .ErrorMemoryMap
        
        mov di, 0x8004
        add di, 24

        

        .MemoryFindLoop:
            pushad
            
            mov eax, 0xE820
            mov edx, 0x534D4150
            mov ecx, 24
            mov [es:di + 20], dword 1 ; ask for ACPI v3 
            int 0x15
            jc short .ErrorMemoryMap
            mov [(.TmpEbxStore-MallocMemoryMapEntry+0x7e00)], dword ebx
            popad
            add di, 24
            mov ebx, [(.TmpEbxStore-MallocMemoryMapEntry+0x7e00)]
            cmp ebx, 0
            je short .exit
            mov eax, 0xb8002
            inc byte [eax]
            jmp short .MemoryFindLoop


        .exit:
            cli
            mov eax, 0xb8000
            mov [ds:eax], byte 'D'
            lgdt [(gdt_ptr-MallocMemoryMapEntry)+0x7e00]

            

            mov eax, cr0
            or eax, 1
            mov cr0, eax
            jmp 0x08:((.ProtectedModeEntry-MallocMemoryMapEntry)+0x7e00)
        
        cli
        hlt
        .ErrorMemoryMap:
            mov eax, 0xb8000
            mov [eax], byte 'M'
            mov [eax+2], byte 'T'
            mov [eax+4], byte 'E'
            mov [eax+6], byte '1'
            mov[eax+8], byte '5'
            cli 
            hlt
        .TmpEbxStore dd 0



    [bits 32]
    .ProtectedModeEntry:
        mov ax, 0x10
        mov ds, ax
        mov es, ax
        mov ss, ax
        mov fs, ax
        mov gs, ax

        mov esp, [StartStack]

        popad
        popf
        ret

        jmp $


StartStack dd 0

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

Cr0Store dd 0
Cr3Store dd 0

LidtRealMode:
    dw 0x3ff
    dd 0

MallocMemoryMapEnd:

MallocMemoryMapFuncSize equ MallocMemoryMapEnd-MallocMemoryMapEntry

global CopyMallocFunc
CopyMallocFunc:
    lgdt [gdt_ptr]
    mov edi, MallocMemoryMapEntry
    mov esi, 0x7e00
    mov ecx, MallocMemoryMapFuncSize

    .CopyLoop:

        mov al, [edi]
        mov [esi], al
        inc esi
        inc edi
        dec ecx
        jnz .CopyLoop

    ret

