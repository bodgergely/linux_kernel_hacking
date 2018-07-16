https://github.com/surajx/qemu-arm-linux/wiki/Compile-Linux,-BusyBox-for-ARM-and-load-it-using-QEMU

1. Compiling Linux from Source for ARM architecture
===================================================

wget https://www.kernel.org/pub/linux/kernel/v3.0/linux-3.10.tar.bz2
tar xjvf linux-3.10.tar.bz2

sudo apt-get install gcc-arm-linux-gnueabi

 export ARCH=arm
 export CROSS_COMPILE=arm-linux-gnueabi-


 Configure Linux Build system to compile for the versatile express family of boards.

 cd linux-3.10
 make vexpress_defconfig


Actually Build the Linux Kernel Code
    make -j 4 all



2. Compiling BusyBox from source for ARM architecture
=====================================================


Get BusyBox source
    wget http://www.busybox.net/downloads/busybox-1.21.1.tar.bz2
Extract source from Gzipped tarball
    tar xjvf busybox-1.21.1.tar.bz2
Configure the BusyBox Build system using the default configurations.
    cd busybox-1.21.1
    make defconfig
Additionally use a GUI driven build configuration settings page to tell BusyBox to compile everything statically and leave out certain unwanted and troublesome modules. make menuconfig Traverse in the GUI

Busybox Settings ==> Build Options
SELECT Build BusyBox as a static binary(no shared libs)
If you are getting an error that curses.h is missing install ncurses-dev package.

    sudo apt-get install libncurses5-dev
Actually Build BusyBox Code
    make -j 4 install
Once the build is complete, a folder named _install is created. This folder contains a bare structure of the linux root file system. As you can see some important folder like proc, dev, sys etc are missing. So lets go ahead and create them.

    cd _install
    mkdir proc sys dev etc etc/init.d
It is not enough that we just create the special directories, we have to tell the kernel to mount special services to their respective directories.

Create etc/init.d/rcS file and enter the following shell code
#!/bin/sh
mount -t proc none /proc
mount -t sysfs none /sys
/sbin/mdev -s
/sbin/init is usually the first program run by the linux kernel and its default behaviour is to execute the /etc/init.d/rcS file.

Mark rcS file as executable
    chmod +x etc/init.d/rcS  
Copy our Custom Memory Device Driver to filesystem (Optional)
Since we are planning to install our simple memory driver on the emulated ARM Linux system, copy the driver files to any folder in this location, preferably create a new one.

Copy Driver .ko to a new folder, memDriver
    mkdir memDrive/
    cp <path to driver files> memDriver/
Create the root filesystem image with the cpio tool.
    find . | cpio -o --format=newc > ../rootfs.img
The root FileSystem should be create by the name rootfs.img inside the busybox-1.21.1 folder.


3. Running Linux with BusyBox on Linux for ARM on QEMU
======================================================

Installing QEMU
    sudo apt-get install qemu
Start QEMU for ARM using our custom, Kernel and BusyBox.
    qemu-system-arm -M vexpress-a9 -m 256M -kernel linux-3.10/arch/arm/boot/zImage -initrd busybox-1.21.1/rootfs.img -append "root=/dev/ram rdinit=/sbin/init"
A QEMU window should open up with kernel initialization messages and finally a message asking, press Enter to activate console. When you hit enter a root prompt is received and now you are running Linux Kernel on an emulated ARM processor.



Create a character device file with Major Number as 60 and minor number as 0.
    mknod /dev/mymem c 60 0
Assign full permission to /dev/mymem
    chmod 777 /dev/mymem
Insert our driver module into the kernel.
    cd memDriver/
    insmod memory.ko
printk messages in module_init function should be now seen in dmesg|tail

Write to device
echo -n 4 > /dev/mymem
Read from device
cat /dev/mymem


