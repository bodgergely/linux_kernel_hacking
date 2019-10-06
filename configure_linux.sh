LINUX_SRC=$WORKSPACE/linux/
BUSYBOX_DIR=$HOME/busybox

BRANCH=master
mkdir -p $BUSYBOX_DIR/build/linux-$BRANCH 

cd $LINUX_SRC
make O=$BUSYBOX_DIR/build/linux-$BRANCH x86_64_defconfig
#make O=$BUSYBOX_DIR/build/linux-$BRANCH i386_defconfig
#below is to have menu config
#make O=$BUSYBOX_DIR/build/linux-3.7.1 menuconfig
#make O=$BUSYBOX_DIR/build/linux-x86-basic kvmconfig
echo "CONFIG_DEBUG_INFO=y" >> $BUSYBOX_DIR/build/linux-$BRANCH/.config
echo "CONFIG_8139CP=y" >> $BUSYBOX_DIR/build/linux-$BRANCH/.config
vim $BUSYBOX_DIR/build/linux-$BRANCH/.config
