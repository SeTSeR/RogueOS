DESTDIR=binary_x86
SRCDIR=.

kasm.o:
	yasm -f elf32 $(SRCDIR)/kernel.asm -o $(DESTDIR)/kasm.o

kc.o:
	gcc -fno-stack-protector -m32 -c $(SRCDIR)/kernel.c -o $(DESTDIR)/kc.o

kernel: kasm.o kc.o
	ld -m elf_i386 -T link.ld -o $(DESTDIR)/kernel $(DESTDIR)/kasm.o $(DESTDIR)/kc.o

clean:
	rm $(DESTDIR)/kasm.o $(DESTDIR)/kc.o

run: kernel
	qemu-system-i386 -kernel $(DESTDIR)/kernel
