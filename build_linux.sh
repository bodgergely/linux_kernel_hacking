LINUX_SRC=$HOME/linux/3.7.1/linux-3.7.1/
BUSYBOX_DIR=$HOME/busybox

cd $LINUX_SRC

make O=$BUSYBOX_DIR/build/linux-3.7.1 -j4
