[bits 32]

db 'JOSK'

section .data
global BIOSDiskNumber
BIOSDiskNumber db 0

section .text

extern KernelEntry
global _start
_start:
    mov [BIOSDiskNumber], byte dl
    mov [0xb8000], byte 'A'
    call KernelEntry
    cli
    hlt

    jmp $

