#!/bin/bash
# BRANCH=master
BRANCH=`cat branch-linux`

if [ ! -z $1 ]
then
    BRANCH=$1
fi


qemu-system-x86_64 \
  -kernel ~/busybox/build/linux-$BRANCH/arch/x86_64/boot/bzImage \
  -initrd ~/busybox/build/initramfs-busybox-x86.cpio.gz \
  -nographic -append "console=ttyS0 nokaslr" \
  -s
