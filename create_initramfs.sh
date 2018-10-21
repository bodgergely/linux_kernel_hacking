export TOP=~/busybox/
rm -rf $TOP/build/initramfs
rm $TOP/build/initramfs-busybox-x86.cpio.gz

#create the initramfs
mkdir -p $TOP/build/initramfs/busybox-x86
cd $TOP/build/initramfs/busybox-x86
mkdir -pv {bin,sbin,etc,proc,sys,usr/{bin,sbin}}
cp -av $TOP/build/busybox-x86/_install/* .
#create the init script
touch init
echo "#!/bin/sh" >> init
echo "mount -t proc none /proc" >> init
echo "mount -t sysfs none /sys" >> init
echo "echo -e \"\nBoot took $(cut -d' ' -f1 /proc/uptime) seconds\n\"" >> init
echo "exec /bin/sh" >> init
chmod +x init

# create the compressed initramfs
find . -print0 \
   | cpio --null -ov --format=newc \
   | gzip -9 > $TOP/build/initramfs-busybox-x86.cpio.gz
