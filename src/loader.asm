[org 0x500]

; hello!
mov si, init_message
call print

xchg bx, bx
; memory checkout
call memory_checkout

jmp $



; ARDS: Address Range Descriptor Structure
; struct {
;       BaseAddrLow   low 32 bits of base address of memory
;       BaseAddrHigh  high 32 bits of base address of memory
;       LengthLow     low 32 bits of memory length
;       LengthHigh    high 32 bits of memory length
;       Type          type of the section(1: available, 2: unavailable)
;}
memory_checkout:
    ; int 0x15 args:
    ;   eax: function number, 0xe820
    ;   ebx: which ards should be returned, 0x0 first, auto change to next by bios
    ;   es: segment base address
    ;   edi: buffer address that store ards, perhapse size = 0
    ;   ecx: sizeof ards, 0x14 bytes
    ;   edx: signature, 0x534d4150
    ;
    ; int 0x15 return:
    ;   cf: 0x0 success, 0x1 failed
    ;   eax: 0x534d4150
    ;   es: segment base address
    ;   edi: buffer address that store ards, should contain some ards
    ;   ecx: sizeof ards, 0x14 bytes
    ;   ebx: which ards should be returned, 0x0 first, auto change to next by bios, should not be care
    ;           ebx = 0 refers to all ards has been loaded
    mov ax, 0
    mov es, ax
    mov di, ards
    mov ecx, 0x14
    mov edx, 0x534d4150
    xor ebx, ebx

    .check:
        mov eax, 0xe820
        int 0x15

        jc memory_checkout_failed
        xor eax, 0x534d4150
        cmp eax, 0
        jnz memory_checkout_failed

        add di, 0x14
        add word [ards_cnt], 1
        cmp ebx, 0
        jnz .check

    mov si, memory_checkout_success_message
    call print
    ret

init_message:
    db "Hello, os world!", 10, 13, 0  ; 10: /n 13: /r

memory_checkout_failed:
    mov si, memory_checkout_failed_message
    call print
    jmp $

memory_checkout_message:
    db "Memory checkout...", 10, 13, 0

memory_checkout_failed_message:
    db "Memory checkout failed!", 10, 13, 0

memory_checkout_success_message:
    db "Memory checkout success!", 10, 13, 0

print:
    push ax
    mov ah, 0xe
    .write:
        mov al, [si]
        cmp al, 0
        jz .done
        int 0x10
        inc si
        jmp .write
    .done:
        pop ax
        ret

ards_cnt:
    dw 0

ards: