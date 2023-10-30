IMG=image/master.img
CXX=nasm

run: build
	bochs -q -f bochs/bochsrc -unlock

build: $(IMG)

%.bin: src/%.asm
	$(CXX) -o $@ $<

$(IMG): boot.bin loader.bin create_img
	dd if=$(word 1, $^) of=$@ bs=512 count=1 conv=notrunc
	dd if=$(word 2, $^) of=$@ bs=512 count=4 seek=2 conv=notrunc


create_img:
ifeq ($(wildcard $(IMG)),)
	bximage -q -hd=16 -func=create -sectsize=512 -imgmode=flat $(IMG)
endif



.PHONY: clean
clean:
	rm -rf *.bin *.o *.lock *.ini
	rm -rf image/master.img
	bximage -q -hd=16 -func=create -sectsize=512 -imgmode=flat $(IMG)

