#!/bin/bash
set -e
export TOP=~/busybox/
rm -f ~/busybox/build/initramfs/busybox-x86/bin/hello_world.ko
cp ./hello_world/hello_world.ko ~/busybox/build/initramfs/busybox-x86/bin/
rm ~/busybox/build/initramfs-busybox-x86.cpio.gz
cd ~/busybox/build/initramfs/busybox-x86/
find . -print0 \
   | cpio --null -ov --format=newc \
   | gzip -9 > $TOP/build/initramfs-busybox-x86.cpio.gz
