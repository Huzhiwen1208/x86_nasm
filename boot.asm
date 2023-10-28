[org 0x7c00]
mov ax, 3
int 0x10
GPU equ 0xb800

xchg bx, bx ; 断点

call addtion
jmp $

addtion:
    mov ax, GPU
    mov es, ax
    mov di, 0

    mov byte es:[di], 'H'
    mov byte es:[di + 2], 'u'
    ret
    
times 510 - ($-$$) db 0
dw 0xaa55
