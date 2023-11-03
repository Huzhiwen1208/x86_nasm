[bits 32]

section .text
global readb
readb:
    push ebp
    mov ebp, esp

    xor eax, eax;  eax = 0
    mov edx, [ebp + 8]; entry argument(u16 port) -> edx, [ebp+4] is function return address
    in al, dx

    nop
    nop
    nop 

    leave
    ret

global writeb
writeb:
    push ebp
    mov ebp, esp

    mov edx, [ebp + 8]; entry argument(u16 port) -> edx
    mov eax, [ebp + 12]; entry argument(u8 data) -> eax
    out dx, al

    nop
    nop
    nop

    leave
    ret