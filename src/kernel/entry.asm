[bits 32]
; Entry point: 0x7e00

extern kernel_main

global _start
_start:
    call kernel_main
    jmp $