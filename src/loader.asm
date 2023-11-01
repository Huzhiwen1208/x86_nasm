[org 0x500]

GPU equ 0xb800; 设置GPU的段基址

xchg bx, bx
mov ax, GPU
mov es, ax
mov di, 0

call print

jmp $

print:
    mov ah, 0xe
    mov si, message

    .write:
        mov al, [si]
        cmp al, 0
        jz .done
        int 0x10
        inc si
        jmp .write
    .done:
        ret

message:
    db "Hello, os world!", 10, 13, 0  ; 10: /n 13: /r