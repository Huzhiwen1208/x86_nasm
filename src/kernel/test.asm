[bits 32]
section .text
global test_func
test_func:
    push ebx

    mov eax, [esp+8]
    mov ebx, [esp+12]
    add eax, ebx

    pop ebx
    ret