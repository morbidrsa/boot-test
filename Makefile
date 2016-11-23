CC=gcc
CFLAGS=-Wall -Wextra -Werror -O2
NAME=init

all: initramfs

initramfs:  $(NAME)
	mkdir -p initramfs/{bin,proc,dev,sys,lib64}
	install -m 755 $(NAME) initramfs/bin/
	install -m 755 /lib64/$(shell ldd init | grep -E -o "libc.so.[0-9]" | head -1) initramfs/lib64/
	install -m 755 /lib64/$(shell ldd init | grep -o -E "ld-linux-x86-64.so.[0-9]+") initramfs/lib64/
	cd initramfs && find . | cpio -H newc -o | gzip > ../initramfs.img

$(NAME): init.c boot-test.h test.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	rm -f $(NAME) *.o initramfs.*
	rm -rf initramfs

.PHONY: clean
