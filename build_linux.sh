LINUX_SRC=$WORKSPACE/linux
BUSYBOX_DIR=$HOME/busybox

cd $LINUX_SRC

make O=$BUSYBOX_DIR/build/linux-x86-basic -j4
