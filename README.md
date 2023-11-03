## Env
```shell
brew install x86_64-elf x86_64-elf-gcc llvm nasm bochs sdl
```

## Problem
- `.s` file compiled by `x86_64-elf-gcc` is not available, should use arg `-mno-sse`
- gdb debug is not available, shoule use arg `-g` in `nasm` and `x86_64-elf-gcc`