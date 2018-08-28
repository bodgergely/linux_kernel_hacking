Source
======
http://ncmiller.github.io/2016/05/14/linux-and-qemu.html

Content
=======

Fast linux kernel testing with qemu
This is the process I followed on my Fedora 23 host machine to build a small/minimal vanilla Linux kernel and test in Qemu (based on this blog post). This will provide a safe sandbox in which to test kernel changes, and is generally faster than developing natively on the host machine. Qemu will boot the kernel image directly in the emulated system.

sudo apt-get install qemu-system

Install required build tools on host machine
dnf install ncurses-devel kernel-devel kernel-headers openssl-devel
dnf install gcc dnf install gcc-c++ glibc-static
dnf install git dnf install qemu
Prepare a working space for kernel development
cd $HOME
mkdir kdev
TOP=$HOME/kdev
Download source code
Download stable busybox, which will provide a basic set of tools for the kernel-under-test:

wget http://busybox.net/downloads/busybox-1.24.2.tar.bz2
tar xvf busybox-1.24.2.tar.bz2
rm busybox-1.24.2.tar.bz2
Clone Linux kernel:

git clone https://github.com/torvalds/linux.git
Configure busybox
cd $TOP/busybox-1.24.2
mkdir -p $TOP/build/busybox-x86
make O=$TOP/build/busybox-x86 defconfig
make O=$TOP/build/busybox-x86 menuconfig
Enable building BusyBox as a static binary:

-> Busybox Settings
  -> Build Options
[*] Build BusyBox as a static binary (no shared libs)
[] Support RPC services - disable it if you get compilation error due to some bindport
Build and install busybox (local install, no sudo required)
cd $TOP/build/busybox-x86
make -j2
make install
Create minimal filesystem
mkdir -p $TOP/build/initramfs/busybox-x86
cd $TOP/build/initramfs/busybox-x86
mkdir -pv {bin,sbin,etc,proc,sys,usr/{bin,sbin}}
cp -av $TOP/build/busybox-x86/_install/* .
Create simple init script
Create a file called init with the following contents:

#!/bin/sh

mount -t proc none /proc
mount -t sysfs none /sys

echo -e "\nBoot took $(cut -d' ' -f1 /proc/uptime) seconds\n"

exec /bin/sh
Make it executable:

chmod +x init
Generate initramfs
find . -print0 \
   | cpio --null -ov --format=newc \
   | gzip -9 > $TOP/build/initramfs-busybox-x86.cpio.gz
Configure Linux using simple defconfig and kvm options
cd $TOP/linux
make O=$TOP/build/linux-x86-basic x86_64_defconfig
make O=$TOP/build/linux-x86-basic kvmconfig
Build Linux
make O=$TOP/build/linux-x86-basic -j2
Boot with Qemu in seconds!
cd $TOP
qemu-system-x86_64 \
  -kernel build/linux-x86-basic/arch/x86_64/boot/bzImage \
  -initrd build/initramfs-busybox-x86.cpio.gz \
  -nographic -append "console=ttyS0 nokaslr" \
  -enable-kvm
