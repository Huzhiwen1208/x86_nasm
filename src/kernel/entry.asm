[bits 32]
; Entry point: 0x7e00
global _start
_start:
    mov byte [0xb8000], 'K'

    jmp $