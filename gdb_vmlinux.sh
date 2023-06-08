#!/bin/bash

# helper functions defined in gdb-kernel-debugging.rst in Linux source code
BRANCH=`cat branch-linux`

if [ ! -z $1 ]
then
    BRANCH=$1
fi

gdb --command qemu.gdb ~/busybox/build/linux-$BRANCH/vmlinux

# Use below command in the booted virtual machine to shut it off
# $poweroff -f
