LINUX_SRC=$WORKSPACE/linux
BUSYBOX_DIR=$HOME/busybox

cd $LINUX_SRC

#make O=$BUSYBOX_DIR/build/linux-x86-basic x86_64_defconfig
make O=$BUSYBOX_DIR/build/linux-x86-basic i386_defconfig
#make O=$BUSYBOX_DIR/build/linux-x86-basic kvmconfig
