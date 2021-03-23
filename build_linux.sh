#!/bin/bash
# BRANCH=master
BRANCH=`cat branch-linux`

if [ ! -z $1 ]
then
    BRANCH=$1
fi

LINUX_SRC=$WORKSPACE/linux/
BUSYBOX_DIR=$HOME/busybox

cd $LINUX_SRC

make O=$BUSYBOX_DIR/build/linux-$BRANCH -j`nproc`
