obj-m := hello_world.o
hello_world-objs := main_hello.o

KERNEL_HEADERS_LIB=/lib/modules/$(shell uname -r)/build


all:
	make $(MAKE_FLAG) -C $(KERNEL_HEADERS_LIB) M=$(PWD) modules

clean:
	make -C $(KERNEL_HEADERS_LIB) M=$(PWD) clean
