LINUX_SRC=$WORKSPACE/linux/
BUSYBOX_DIR=$HOME/busybox

BRANCH=master

cd $LINUX_SRC

make O=$BUSYBOX_DIR/build/linux-$BRANCH -j4
