TARGET=boot.bin
IMG=image/master.img
OBJ=src/boot.asm
CXX=nasm

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^

$(IMG): $(TARGET)
	dd if=$< of=$@ bs=512 count=1 conv=notrunc

run: $(IMG)
	bochs -q -f bochs/bochsrc -unlock

%.bin: src/%.asm
	$(CXX) -o $@ $<

.PHONY: clean
clean:
	rm -rf *.bin *.o *.lock *.ini
