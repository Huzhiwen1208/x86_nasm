[bits 32]

section .text
global readb
readb:
    push ebp
    mov ebp, esp

    xor eax, eax;  eax = 0
    ; entry argument(u16 port) -> edx, [ebp+4] is function return address, [ebp] is ebp
    mov edx, [ebp + 8]
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

global readw
readw:
    push ebp
    mov ebp, esp

    xor eax, eax;  eax = 0
    ; entry argument(u16 port) -> edx, [ebp+4] is function return address, [ebp] is ebp
    mov edx, [ebp + 8]
    in ax, dx

    nop
    nop
    nop 

    leave
    ret

global writew
writew:
    push ebp
    mov ebp, esp

    mov edx, [ebp + 8]; entry argument(u16 port) -> edx
    mov eax, [ebp + 12]; entry argument(u16 data) -> eax
    out dx, ax

    nop
    nop
    nop

    leave
    ret