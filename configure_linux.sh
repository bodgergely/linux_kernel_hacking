LINUX_SRC=$HOME/linux/3.7.1/linux-3.7.1/
BUSYBOX_DIR=$HOME/busybox

cd $LINUX_SRC

#make O=$BUSYBOX_DIR/build/linux-x86-basic x86_64_defconfig
make O=$BUSYBOX_DIR/build/linux-3.7.1 i386_defconfig
#below is to have menu config
#make O=$BUSYBOX_DIR/build/linux-3.7.1 menuconfig
#make O=$BUSYBOX_DIR/build/linux-x86-basic kvmconfig
