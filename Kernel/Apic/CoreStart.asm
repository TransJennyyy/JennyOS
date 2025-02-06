
extern CurrentCoreStarting
extern IdtrPTR
extern CoreStateList
extern CoreStackPtrs
extern CoreCEntry
[bits 16]

global CoreStartEntry
CoreStartEntry: 
    cli
   
    mov eax, (gdt_ptr-CoreStartEntry)+0x8000
    lgdt [eax]

    in al, 0x92
    or al, 2
    out 0x92, al


    mov eax, cr0
    or eax, 1
    mov cr0, eax


    jmp 08h:((AfterProtectedMode-CoreStartEntry)+0x8000)
    
    



gdt_ptr:
    dw gdt_end - gdt_start - 1  ; Limit (Size of GDT - 1)
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

gdt_end:

[bits 32]
AfterProtectedMode:
    mov ax, 0x10
    mov es, ax
    mov ds, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    jmp 08h:CoreEntrySpace


CoreStartEntryEnd:
global CoreStartEntrySize
CoreStartEntrySize dd (CoreStartEntryEnd-CoreStartEntry) 

CoreEntrySpace:
    mov [0xb8000], byte 'A'
    
    mov esi, [CoreStackPtrs]

    mov ebx, [CurrentCoreStarting]
    mov eax, 4
    mul ebx
    add esi, eax    

    mov esp, [esi]
    mov ebp, esp

    xor eax, eax
    mov dr0, eax
    mov dr1, eax
    mov dr2, eax
    mov dr3, eax
    mov dr7, eax

    mov eax, [IdtrPTR]
    lidt [eax]
    sti

    call CoreCEntry
    jmp $
    cli
    hlt
