UEFI_ROOTLBA = 0x20000	# Corresponds to LBA of the first block of the second partition (UEFI)

BIOS_ROOTLBA = 201		# Corresponds to LBA of the first block the filesystem will be written to



all: filesystem


# desktop
desktop/desktop_o:
	$(MAKE) -C desktop


# filesystem
filesystem: desktop/desktop_o
	g++ fsmap.c++ -o fsmap
	./fsmap ${UEFI_ROOTLBA}

bios_filesystem: desktop/desktop_o
	g++ fsmap.c++ -o fsmap
	./fsmap ${BIOS_ROOTLBA}

clean:
	find . -name '*.o' -type f -delete
	rm -fr *.bin *.dis filesystem
	rm -fr fsmap