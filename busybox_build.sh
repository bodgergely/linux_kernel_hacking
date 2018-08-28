#export CFLAGS=-m32
rm -rf $HOME/busybox/build/busybox-x86
mkdir $HOME/busybox/build/busybox-x86
cd $HOME/busybox/busybox-1.21.0
make clean
make O=$HOME/busybox/build/busybox-x86 defconfig
# build busybox as static library in Build Options/ also unselect in Networking the RPC support
make menuconfig
make -j4
cd $HOME/workspace/hacking/kernel_hacking
./create_initramfs.sh
