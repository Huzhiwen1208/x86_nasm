; 1. 时钟中断处理函数注册
; 2. 打开8259的中断屏蔽字
; 3. 打开CPU的中断允许标志IF=1
; 4. 处理完中断要表明已处理完此次中断，方便下次中断触发，否则不会触发下次中断
[org 0x7c00]
mov ax, 3
int 0x10
GPU equ 0xb800
xchg bx, bx ; 断点

; --注册时钟中断处理函数, 中断向量表
mov word [0x08 * 4], clock_i
mov word [0x08 * 4 + 2], 0
; --打开8259的中断屏蔽字
mov al, 0b11111110
out 0x21, al
; --打开CPU的中断允许标志IF=1
sti

mov bx, 5
mov dh, 'A'; flash参数
loop_switch:
    call flash
    jmp loop_switch

jmp $

clock_i:
    push ax
    push es
    push bx 
    push dx 

    mov dh, 'C'; flash参数
    mov bx, 7
    call flash
    mov al, 0x20
    out 0x20, al ;告知中断完成

    pop dx
    pop bx
    pop es
    pop ax

    iret; 中断调用返回

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
