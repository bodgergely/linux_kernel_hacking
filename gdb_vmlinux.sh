#!/bin/bash

# helper functions defined in gdb-kernel-debugging.rst in Linux source code
BRANCH=master

if [ ! -z $1 ]
then
    BRANCH=$1
fi

gdb --command qemu.gdb ~/busybox/build/linux-$BRANCH/vmlinux

