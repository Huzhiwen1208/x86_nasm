[bits 32]

extern trap_handler
section .text
global __all_traps
__all_traps:
    ; entry trap handler by "int 0x80"
    push ds
    push es
    push fs
    push gs
    pushad

    push edx; arg3
    push ecx; arg2
    push ebx; arg1
    push eax; syscall number
    call trap_handler
    add esp, 4*4

    mov dword [esp + 7*4], eax ; save eax => stack, return value

__restore:
    popad
    pop gs
    pop fs
    pop es
    pop ds

    iret