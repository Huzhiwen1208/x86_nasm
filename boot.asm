[org 0x7c00]
mov ax, 3
int 0x10
GPU equ 0xb800
xchg bx, bx ; 断点

mov bx, 5
mov dh, 'A'; flash参数
loop_switch:
    call flash
    jmp loop_switch

jmp $

flash:
    push ax
    push es
    push bx 
    push dx ; save context

    mov ax, GPU
    mov es, ax ; 显卡段基址写入es

    shl bx, 1; 取图形界面的第dx个字符
    mov dl, [es:bx]
    cmp dl, ' '

    jz .switch_char
    ; switch empty
    mov byte [es:bx], ' '
    jmp .return

    .switch_char:
        mov [es:bx], dh; dh为参数
    .return:
        pop dx
        pop bx
        pop es
        pop ax  ; restore context

        ret

times 510 - ($-$$) db 0
dw 0xaa55
