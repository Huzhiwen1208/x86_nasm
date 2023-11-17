[bits 32]

extern trap_handler
section .text
global __all_traps
global __restore
__all_traps:
    ; entry trap handler by "int 0x80"
    push 0x88888888
    push ds
    push es
    push fs
    push gs
    pushad
    push 0x80

    push edx; arg3
    push ecx; arg2
    push ebx; arg1
    push eax; syscall number
    call trap_handler
    add esp, 4*4

    mov dword [esp + 8*4], eax ; save eax => stack, return value

__restore:
    add esp, 4; pop 0x80

    popad
    pop gs
    pop fs
    pop es
    pop ds

    add esp, 4

    iret