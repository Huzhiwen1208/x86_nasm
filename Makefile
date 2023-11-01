IMG=image/master.img
CXX=nasm

LoaderPath=src/bootloader
KernelPath=src/kernel
ALLKernelASM=$(wildcard $(KernelPath)/*.asm)
ALLKernelOBj=$(ALLKernelASM:.asm=.o)
ELFKernel=$(KernelPath)/kernel.elf
NakedKernel=$(KernelPath)/os.bin
ENTRYPOINT=0x7e00

run: build
	bochs -q -f bochs/bochsrc -unlock

build: $(IMG)

$(KernelPath)/%.o: $(KernelPath)/%.asm
	$(CXX) -f elf32 -o $@ $<

# replace ld, objcopy in linux:binutils with ld.lld, llvm-objcopy in macos:llvm
$(ELFKernel): $(KernelPath)/entry.o
	ld.lld -m elf_i386 -static -Ttext $(ENTRYPOINT) -o $@ $^

$(NakedKernel): $(ELFKernel)
	llvm-objcopy -O binary $< $@

%.bin: $(LoaderPath)/%.asm
	$(CXX) -o $@ $<

$(IMG): boot.bin loader.bin $(NakedKernel) create_img 
	dd if=$(word 1, $^) of=$@ bs=512 count=1 conv=notrunc
	dd if=$(word 2, $^) of=$@ bs=512 count=10 seek=2 conv=notrunc
	dd if=$(NakedKernel) of=$@ bs=512 count=200 seek=12 conv=notrunc

create_img:
ifeq ($(wildcard $(IMG)),)
	bximage -q -hd=16 -func=create -sectsize=512 -imgmode=flat $(IMG)
endif


.PHONY: clean
clean:
	rm -rf *.bin *.o *.lock *.ini
	rm -rf image/master.img
	rm -rf $(KernelPath)/*.o $(KernelPath)/*.bin $(KernelPath)/*.elf
	bximage -q -hd=16 -func=create -sectsize=512 -imgmode=flat $(IMG)

