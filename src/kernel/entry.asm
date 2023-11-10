[bits 32]
; Entry point: 0x7e00, indicated by the ld.lld linker script

magic equ 0xe85250d6
arch equ 0
length equ multiboot2_header_end - multiboot2_header

checksum equ -(magic + arch + length)


section .multiboot2
multiboot2_header:
    dd magic
    dd arch
    dd length
    dd checksum

    ; tags
    dw 0
    dw 0
    dd 8
multiboot2_header_end:

section .text
extern kernel_main
extern memory_init

global _start
_start:
    call memory_init
    call kernel_main
    jmp $