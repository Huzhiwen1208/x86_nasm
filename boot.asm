[org 0x7c00]
mov ax, 3
int 0x10
GPU equ 0xb800

xchg bx, bx ; 断点

; 注册系统调用内中断
mov word [0x80 * 4], print ; 段偏移
mov word [0x80 * 4 + 2], 0 ; 段基址
; ---

int 0x80

jmp $

print:
    push ax
    push es
    push di

    mov ax, GPU
    mov es, ax
    mov di, 0
    mov byte [es:di], 'H'


    pop di
    pop es
    pop ax
    ret 

times 510 - ($-$$) db 0
dw 0xaa55
