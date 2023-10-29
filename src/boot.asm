; 1. 时钟中断处理函数注册
; 2. 打开8259的中断屏蔽字
; 3. 打开CPU的中断允许标志IF=1
; 4. 处理完中断要表明已处理完此次中断，方便下次中断触发，否则不会触发下次中断
[org 0x7c00]
mov ax, 3
int 0x10
GPU equ 0xb800
xchg bx, bx ; 断点

mov dx, 0x1f2
mov al, 1
out dx, al; 设置扇区数量为al: byte [0x1f2]=1

; 设置扇区号为0
mov al, 0
inc dx 
out dx, al

inc dx
out dx, al

inc dx
out dx, al

inc dx
mov al, 0b11100000; 低四位为扇区号使用
out dx, al

inc dx
mov al, 0x20
out dx, al; 设置读硬盘模式

_check_disk_state:
    nop
    nop 
    nop
    
    in al, dx
    test al, 0x80; 0x80为忙标志位
    jnz _check_disk_state; 忙，继续检查

mov ax, 0x100
mov es, ax
mov di, 0
mov dx, 0x1f0; 设置读端口

_read:
    nop
    nop
    nop

    in ax, dx
    mov [es:di], ax; 写入0x100段
    add di, 2
    cmp di, 512; 是否读取一整块了？
    jnz _read; 没有读取一整块，继续读取

jmp $

times 510 - ($-$$) db 0
dw 0xaa55