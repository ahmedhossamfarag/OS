

%.bin : %.asm
	nasm $< -f bin -o $@

all: filesystem

# shell
shell/shell.o:
	$(MAKE) -C shell


# filesystem
filesystem: fsmap.bin shell/shell.o
	dd if=fsmap.bin of=filesystem bs=512 seek=0 conv=notrunc
	dd if=shell/shell.o of=filesystem bs=512 seek=2 conv=notrunc


clean:
	find . -name '*.o' -type f -delete
	rm -fr *.bin *.dis filesystem