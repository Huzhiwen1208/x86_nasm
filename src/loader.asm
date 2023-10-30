[org 0x1000]

GPU equ 0xb800; 设置GPU的段基址

xchg bx, bx
mov ax, GPU
mov es, ax
mov di, 0

mov byte [es:di], 'H'

jmp $