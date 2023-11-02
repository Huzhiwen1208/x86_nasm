IMG=image/master.img
CXX=nasm
GCC=x86_64-elf-gcc
TARGET=src/target

# -mno-sse is nessary in macos, not using high level instruction or register
KernelLdFlags=-m elf_i386 -static
KernelGccFlags=-m32 -fno-builtin -fno-stack-protector -mno-sse
KernelGccFlags+=-nostdinc -nostdlib -fno-pic -fno-pie

BootLoader=src/bootloader
KernelPath=src/kernel
ELFKernel=$(TARGET)/kernel/os.elf
NakedKernel=$(TARGET)/kernel/os.bin
KernelOBJ=$(TARGET)/kernel/entry.o $(TARGET)/kernel/main.o
ENTRYPOINT=0x7e00

SourceCFile=$(KernelPath)/main.c
PreCompiledFile=test/pre_compile.c
CompiledFile=test/assembly.s
OBJFile=test/obj.o
EXEFile=test/exe.elf

run: build
	bochs -q -f bochs/bochsrc -unlock

build: clean $(IMG)

$(TARGET):
ifeq ($(wildcard $(TARGET)),)
	@mkdir -p $(TARGET)/bootloader
	@mkdir -p $(TARGET)/kernel
endif

# test -----
.PHONY: test
test: clean $(EXEFile)

$(PreCompiledFile): $(SourceCFile)
	$(GCC) -m32 -E $(SourceCFile) -I $(KernelPath) > $(PreCompiledFile)

$(CompiledFile): $(PreCompiledFile)
	$(GCC) -m32 -mno-sse -S -o $(CompiledFile) $(PreCompiledFile)

$(OBJFile): $(CompiledFile)
	x86_64-elf-as --32 -o $(OBJFile) $(CompiledFile)

$(EXEFile): $(OBJFile)
	ld.lld $(KernelLdFlags) -e kernel_init -o $@ $(OBJFile)
# ------ test done

# .c, .asm ---> .o ------
$(TARGET)/kernel/%.o: $(KernelPath)/%.c
	$(GCC) $(KernelGccFlags) -c -o $@ $<

$(TARGET)/kernel/%.o: $(KernelPath)/%.asm
	$(CXX) -f elf32 -o $@ $<
# ----- .c, .asm ---> .o

# bootloader  -------
$(TARGET)/bootloader/%.bin: $(BootLoader)/%.asm
	$(CXX) -o $@ $<
# ------ bootloader

# kernel made ----- 
# replace ld, objcopy in linux:binutils with ld.lld, llvm-objcopy in macos:llvm
$(ELFKernel): $(KernelOBJ)
	ld.lld -m elf_i386 -static -Ttext $(ENTRYPOINT) -o $@ $^
$(NakedKernel): $(ELFKernel)
	llvm-objcopy -O binary $< $@
# ------ kernel made

# img made --------
$(IMG): $(TARGET) $(TARGET)/bootloader/boot.bin $(TARGET)/bootloader/loader.bin \
	$(NakedKernel) create_img
	dd if=$(word 2, $^) of=$@ bs=512 count=1 conv=notrunc
	dd if=$(word 3, $^) of=$@ bs=512 count=10 seek=2 conv=notrunc
	dd if=$(word 4, $^) of=$@ bs=512 count=200 seek=12 conv=notrunc

create_img:
ifeq ($(wildcard image),)
	@mkdir image
endif
ifeq ($(wildcard $(IMG)),)
	bximage -q -hd=16 -func=create -sectsize=512 -imgmode=flat $(IMG)
endif
# ------- img made

.PHONY: clean
clean:
	rm -rf *.bin *.o *.lock *.ini *.s *.asm
	rm -rf image
	rm -rf test/*
	rm -rf src/target
