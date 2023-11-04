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