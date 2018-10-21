#export CFLAGS=-m32
rm -rf $HOME/busybox/build/busybox-x86
mkdir $HOME/busybox/build/busybox-x86
cd $HOME/busybox/busybox-1.29.3
make clean
make O=$HOME/busybox/build/busybox-x86 defconfig
# build busybox as static library in Build Options/ also unselect in Networking the RPC support
cd $HOME/busybox/build/busybox-x86
make menuconfig
make -j4
make install
cd $HOME/workspace/hacking/kernel_hacking
./create_initramfs.sh
