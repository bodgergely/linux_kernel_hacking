#!/bin/bash
export TOP=~/busybox/
cd $TOP/build/initramfs/busybox-x86
# create the compressed initramfs
find . -print0 \
   | cpio --null -ov --format=newc \
   | gzip -9 > $TOP/build/initramfs-busybox-x86.cpio.gz
