obj-m := kretprobe_example.o
char_dev-objs := kretprobe_example.o

# KERNEL_HEADERS_LIB=/lib/modules/$(shell uname -r)/build
KERNEL_HEADERS_LIB=~/busybox/build/linux-$(shell cat ../../branch-linux)

all:
	make $(MAKE_FLAG) -C $(KERNEL_HEADERS_LIB) M=$(PWD) modules

clean:
	make -C $(KERNEL_HEADERS_LIB) M=$(PWD) clean
