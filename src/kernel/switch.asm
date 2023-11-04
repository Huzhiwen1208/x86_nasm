[bits 32]
section .text

global __switch
__switch:
    push ebp
    push ebx
    push esi
    push edi


    mov eax, [esp + 20]; current pcb
    mov [eax], esp;  save new esp to current pcb

    mov eax, [esp + 24]; next pcb
    mov esp, [eax];  switch esp to next pcb

    pop edi
    pop esi
    pop ebx
    pop ebp

    ret; pop eip, return eip