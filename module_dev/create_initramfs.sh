#!/bin/bash
set -e
if [[ -z $1 ]]
then
    echo Need to provide the module name as \$1 arg!
    exit -1
fi
MODULE=$1

export TOP=~/busybox/
../create_initramfs.sh

cp ./$MODULE/$MODULE.ko $TOP/build/initramfs/busybox-x86/
rm $TOP/build/initramfs-busybox-x86.cpio.gz
cd $TOP/build/initramfs/busybox-x86/
find . -print0 \
   | cpio --null -ov --format=newc \
   | gzip -9 > $TOP/build/initramfs-busybox-x86.cpio.gz
