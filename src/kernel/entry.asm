[bits 32]
; Entry point: 0x7e00, indicated by the ld.lld linker script

extern kernel_main
extern memory_init

global _start
_start:
    call memory_init
    call kernel_main
    jmp $