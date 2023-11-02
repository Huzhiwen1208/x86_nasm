[bits 32]
; Entry point: 0x7e00

extern kernel_init

global _start
_start:
    xchg bx, bx
    call kernel_init
    jmp $