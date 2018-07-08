Source
======
https://mgalgs.github.io/2015/05/16/how-to-build-a-custom-linux-kernel-for-qemu-2015-edition.html

Content
=======

How to Build A Custom Linux Kernel For Qemu (2015 Edition)
16 May 2015
This is an updated version of my Linux Kernel/Qemu tutorial from 2012.

In this tutorial, we’ll be building a custom Linux kernel and userland to run on qemu. We’ll start with something basic and easy, then we’ll whittle it down until we’re booting straight to an infinite loop of nops (ok, not quite that far). The host and target will both be x86.

(UPDATE) 2017-03-29: kernel and busybox versions updated to the latest stable releases.

Preparation
First, create a workspace:

$ TOP=$HOME/teeny-linux
$ mkdir $TOP
Our entire system will be composed of exactly two packages: the Linux kernel and Busybox. Download and extract them now:

$ cd $TOP
$ curl https://cdn.kernel.org/pub/linux/kernel/v4.x/linux-4.10.6.tar.xz | tar xJf -
$ curl https://busybox.net/downloads/busybox-1.26.2.tar.bz2 | tar xjf -
Busybox Userland
The first thing we’ll do is create a minimal userland based on the ever-useful busybox tool. After building busybox, we’ll throw it in a minimal filesystem hierarchy and package it up in an initramfs using cpio.

Let’s go configure busybox now:

$ cd $TOP/busybox-1.26.2
$ mkdir -pv ../obj/busybox-x86
$ make O=../obj/busybox-x86 defconfig
(Note: in the busybox build system, O= means “place build output here”. This allows you to host multiple different configurations out of the same source tree. The Linux kernel follows a similar convention.)

This gives us a basic starting point. We’re going to take the easy way out here and just statically link busybox in order to avoid fiddling with shared libraries. We’ll need to use busybox’s menuconfig interface to enable static linking:

$ make O=../obj/busybox-x86 menuconfig
type /, search for “static”. You’ll see that the option is located at:

-> Busybox Settings
  -> Build Options
[ ] Build BusyBox as a static binary (no shared libs)
Go to that location, select it, save, and exit.

Now build busybox:

$ cd ../obj/busybox-x86
$ make -j2
$ make install
So far so good. With a statically-linked busybox in hand we can build the directory structure for our initramfs:

$ mkdir -pv $TOP/initramfs/x86-busybox
$ cd $TOP/initramfs/x86-busybox
$ mkdir -pv {bin,sbin,etc,proc,sys,usr/{bin,sbin}}
$ cp -av $TOP/obj/busybox-x86/_install/* .
Of course, there’s a lot missing from this skeleton hierarachy that will cause a lot of applications to break (no /etc/passwd, for example), but it’s enough to boot to a shell, so we’ll live with it for the sake of brevity. If you want to flesh it out more you can refer to these sections of Linux From Scratch.

One absolutely critical piece of our userland that’s still missing is an init program. We’ll just write a tiny shell script and use it as our init:

$ vim init
And enter the following:

#!/bin/sh
 
mount -t proc none /proc
mount -t sysfs none /sys
 
echo -e "\nBoot took $(cut -d' ' -f1 /proc/uptime) seconds\n"
 
exec /bin/sh
view rawinit hosted with ❤ by GitHub
and make it executable:

$ chmod +x init
The Gentoo wiki’s Custom Initramfs page is a great reference for building a minimalistic initramfs if you’d like to learn more.

We’re now ready to cpio everything up:

$ find . -print0 \
    | cpio --null -ov --format=newc \
    | gzip -9 > $TOP/obj/initramfs-busybox-x86.cpio.gz
We now have a minimal userland in $TOP/obj/initramfs-busybox-x86.cpio.gz that we can pass to qemu as an initrd (using the -initrd option). But before we can do that we need a kernel…

Linux Kernel
Basic Kernel Config
For our not-yet-trimmed-down baseline, let’s build a kernel using the default x86_64 configuration that ships with the kernel tree. Apply the configuration like so:

$ cd $TOP/linux-4.10.6
$ make O=../obj/linux-x86-basic x86_64_defconfig
We can also merge in a few config options that improve performance/functionality of kvm guests with:

$ make O=../obj/linux-x86-basic kvmconfig
The kernel is now configured and ready to build. Go ahead and build it:

$ make O=../obj/linux-x86-basic -j2
Now that we have a kernel and a userland, we’re ready to boot! You can use qemu-system-x86_64 to try out your new system:

$ cd $TOP
$ qemu-system-x86_64 \
    -kernel obj/linux-x86-basic/arch/x86_64/boot/bzImage \
    -initrd obj/initramfs-busybox-x86.cpio.gz \
    -nographic -append "console=ttyS0"
Exit the VM by hitting Ctl-a c then typing “quit” at the qemu monitor shell.

If your host processor and kernel have virtualization extensions you can add the -enable-kvm flag to really speed things up:

$ qemu-system-x86_64 \
    -kernel obj/linux-x86-basic/arch/x86_64/boot/bzImage \
    -initrd obj/initramfs-busybox-x86.cpio.gz \
    -nographic -append "console=ttyS0" -enable-kvm
Smaller Kernel Config
That’s great and all, but if we really just want a tiny system with nothing but busybox on it we can remove a bunch of stuff from our kernel. By trimming down our kernel config we can reduce the size of our kernel image and reduce boot time.

Let’s try using the kernel’s Kbuild defaults as our baseline. The Kbuild defaults are generally quite conservative since Linus Torvalds has declared that in the kernel unless the feature cures cancer, it’s not on by default, as opposed to the x86_64_defconfig which is meant to provide a lot of generally useful features and work on a wide variety of x86 targets.

You can apply this more conservative configuration based on the Kbuild defaults by using the alldefconfig target:

$ cd $TOP/linux-4.10.6
$ make O=../obj/linux-x86-alldefconfig alldefconfig
We need to enable a few more options in order to actually be able to use this configuration.

First, we need to enable a serial driver so that we can get a serial console. Run your preferred kernel configurator (I like nconfig, but you can use menuconfig, xconfig, etc.):

$ make O=../obj/linux-x86-alldefconfig nconfig
Navigate to:

-> Device Drivers
  -> Character devices
    -> Serial drivers
and enable the following options:

[*] 8250/16550 and compatible serial support
[*] Console on 8250/16550 and compatible serial port
We also need to enable initramfs support, so that we can actually boot our userland. Go to:

-> General setup
and select:

[*] Initial RAM filesystem and RAM disk (initramfs/initrd) support
You can also deselect all of the decompressors except gzip, since that’s what we’re using.

Finally, enable some features for kvm guests (not actually necessary to get the system booting, but hey):

$ make O=../obj/linux-x86-alldefconfig kvmconfig
And build:

$ make O=../obj/linux-x86-alldefconfig -j2
We now have a much smaller kernel image:

$ (cd $TOP; du -hs obj/linux-x86-*/vmlinux)
6.5M    obj/linux-x86-alldefconfig/vmlinux
19M     obj/linux-x86-basic/vmlinux
Now you can boot the new kernel (with our same userspace):

$ qemu-system-x86_64 \
    -kernel obj/linux-x86-alldefconfig/arch/x86_64/boot/bzImage \
    -initrd obj/initramfs-busybox-x86.cpio.gz \
    -nographic -append "console=ttyS0" -enable-kvm
Not only is it smaller than the last one, but it boots faster too!

Configuration	Boot time (seconds)
x86_64_defconfig + kvmconfig	1.73
alldefconfig + custom stuff + kvmconfig	0.61
Smallest Kernel Config
We saw a 3x decrease in kernel image size and boot time by using a smaller set of default options. But how much smaller and “faster” can we go?

Let’s prune the image down even further by starting with absolutely nothing. The kernel build system has a make target for this: allnoconfig. Let’s create a new configuration based on that:

$ cd $TOP/linux-4.10.6
$ make O=$TOP/obj/linux-x86-allnoconfig allnoconfig
Now everything that can be turned off is turned off. This is as low as it goes without hacking up the kernel source. As one might expect, we have a little more work to do in order to get something that actually boots in qemu. There isn’t a ton to do, which is actually pretty incredible.

Fire up your kernel configurator:

$ make O=../obj/linux-x86-allnoconfig nconfig
Here are the options you need to turn on:

[*] 64-bit kernel

-> General setup
  -> Configure standard kernel features
[*] Enable support for printk

-> General setup
[*] Initial RAM filesystem and RAM disk (initramfs/initrd) support

-> Executable file formats / Emulations
[*] Kernel support for ELF binaries
[*] Kernel support for scripts starting with #!

-> Device Drivers
  -> Character devices
[*] Enable TTY

-> Device Drivers
  -> Character devices
    -> Serial drivers
[*] 8250/16550 and compatible serial support
[*]   Console on 8250/16550 and compatible serial port

-> File systems
  -> Pseudo filesystems
[*] /proc file system support
[*] sysfs file system support
In order to keep things truly tiny, we’ll skip make kvmconfig. Build it:

$ make O=../obj/linux-x86-allnoconfig -j2
The resulting image is quite a bit smaller than our last one, and way smaller than the one based on x86_64_defconfig:

$ (cd $TOP; du -hs obj/linux-x86-*/vmlinux)
6.5M    obj/linux-x86-alldefconfig/vmlinux
2.7M    obj/linux-x86-allnoconfig/vmlinux
19M     obj/linux-x86-basic/vmlinux
Adding make kvmconfig increases the image size to 5M, so allnoconfig isn’t actually a huge win in terms of size against alldefconfig.

And boot it:

$ qemu-system-x86_64 \
    -kernel obj/linux-x86-allnoconfig/arch/x86_64/boot/bzImage \
    -initrd obj/initramfs-busybox-x86.cpio.gz \
    -nographic -append "console=ttyS0" -enable-kvm
Our new tiniest kernel boots about twice as fast as the alldefconfig one and about 5x as fast as the one based on x86_64_defconfig. Adding kvmconfig didn’t really affect boot time.

Configuration	Boot time (seconds)
x86_64_defconfig + kvmconfig	1.73
alldefconfig + custom stuff + kvmconfig	0.61
allnoconfig + custom stuff	0.36
allnoconfig + custom stuff + kvmconfig	0.39
Conclusion
The most obvious application for this type of work is in the embedded space. However, I could see how it might also be beneficial in elastic cloud computing to reduce boot times and memory footprint. Please leave a comment if you’re aware of anyone doing this in “the cloud”!

If nothing else it’s an interesting exercise! :)


