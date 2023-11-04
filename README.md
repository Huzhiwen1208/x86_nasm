## Env
```shell
brew install x86_64-elf x86_64-elf-gcc llvm nasm bochs sdl
```

## Problem
- `.s` file compiled by `x86_64-elf-gcc` is not available, should use arg `-mno-sse`
- gdb debug is not available, shoule use arg `-g` in `nasm` and `x86_64-elf-gcc`


## TASK
### ABI
| register | saver  |
| -------- | ------ |
| eax      | caller |
| ecx      | caller |
| edx      | caller |
| -        | -      |
| ebx      | callee |
| esi      | callee |
| edi      | callee |
| ebp      | callee |
| esp      | callee |
### Memory Layout
| Address offset | Value    | Description | Mark              |
| -------------- | -------- | ----------- | ----------------- |
| 0x1000         | Page Top | -           | -                 |
| 0x0ffc         | eip      | Entry Point | Application Entry |
| 0x0ff8         | ebp      | Stack Frame | -                 |
| 0x0ff4         | ebx      | Data Point  | -                 |
| 0x0ff0         | esi      | -           | -                 |
| 0x0fec         | edi      | -           | esp ->            |
| ...            | ...      | ...         | ...               |
| ...            | ...      | ...         | ...               |
| ...            | ...      | ...         | ...               |
| 0x0004         | other    | -           | pcb--->           |
| 0x0000         | esp      | -           | pcb->             |

## Interrupt
### Internal Interrupt
- Software Interrupt
  - System call
- Exception:
  - Div zero
  - Page Fault
  - Instruction Fault

### External Interrupt
- Clock interrupt
- Keyboard interrupt
- Disk interrupt
  - Sync IO
  - Async IO
  - DMA(Direct Memory access)
  
### Interrupt Vector Table In Kernel(IDT)
> IDT: Interrupt descriptor table
```cpp
// ID: interrupt descriptor
typedef struct interrupt_descriptor {
  u16 offset_low;    // 0 - 15 low bit, offset in segment
  u16 selector;   // code selector
  u8 reserved;    // unused
  u8 type : 4;    // task gate/interrupt gate/trap gate -> 0x0110/0x1110/...
  u8 segment : 1; // 0: system segment, 1: code segment
  u8 DPL : 2;     // DPL
  u8 present : 1; // present in memory?
  u16 offset1;    // 16-31 high bit offset in segment
} _no_align interrupt_descriptor;
```
