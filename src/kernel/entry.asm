[bits 32]
; Entry point: 0x7e00, indicated by the ld.lld linker script

extern kernel_main

global _start
_start:
    call kernel_main
    int 0x80
    jmp $