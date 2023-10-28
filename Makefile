TARGET=boot.bin
IMG=master.img
OBJ=boot.asm
CXX=nasm

$(TARGET): $(OBJ)
	$(CXX) -o $@ $^

$(IMG): $(TARGET)
	dd if=$< of=$@ bs=512 count=1 conv=notrunc

run: $(IMG)
	bochs -q -f bochsrc -unlock

%.bin: %.asm
	$(CXX) -o $@ $<

.PHONY: clean
clean:
	rm -rf *.bin *.o *.lock *.ini
