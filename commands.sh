nasm boot/boot.asm -f bin -o boot.bin
gcc -m32 -c -fno-pie kernel/kernel.c -o kernel.o
ld -m elf_i386 -o kernel.bin -Ttext 0x1000 kernel.o --oformat binary
cat boot.bin kernel.bin > os-image
qemu-system-x86_64 os-image

objdump -d kernel.o > kernel.dis
ndisasm -b 32 kernel.bin > kernel.dis

dd if=inputfile of=outputfile bs=512 seek=10 conv=notrunc
readelf -h shell.bin | grep Type

g++ -m32 -o $@ $^ -z noexecstack
g++ -m32 -shared -fpic -Ishell/include -o libmylib.so shell/mylib.c++
nasm shell/start.asm -f elf -o start.o
g++ -m32 -c -Ishell/include shell/main.c++ -o main.o
g++ -m32 -nostartfiles -o shell.o $^
g++ -m32 -nostdlib -nodefaultlibs -nostartfiles -o $@ $^ -z noexecstack
g++ -m32 -nostdlib -nodefaultlibs -c ${GCC_INCLUDE} $< -o $@

ldd shell/shell.o

dd if=/dev/zero of=myos.qcow2 count=100000
parted myos.qcow2 mklabel gpt
parted myos.qcow2 mkpart primary fat32 1MiB 20MiB
parted myos.qcow2 mkpart primary ext4 20MiB 100%

sudo apt-get install gnu-efi
gcc -I/usr/include/efi -I/usr/include/efi/x86_64 -nostdlib -fpic -fshort-wchar -mno-red-zone -c hello.c -o hello.o
ld -nostdlib -znocombreloc -shared -Bsymbolic -L/usr/lib -T /usr/lib/elf_x86_64_efi.lds hello.o -o hello.efi

qemu-system-x86_64 -device intel-hda -device hda-duplex -smp 4 -m 2048 -drive file=os-image -bios /usr/share/ovmf/OVMF.fd
qemu-system-x86_64 -device intel-hda -device hda-duplex -smp 4 -m 2048 -drive file=os-image,format=raw
nasm $< -f bin -o $@

grub-file --is-x86-multiboot os-image

readelf -x .apsetup ap_setup.o
i686-elf-as -o empty.o /dev/null
objcopy --add-section=.apsetup=ap_setup.bin empty.o ap_setup.o
readelf -x .section_name file.o