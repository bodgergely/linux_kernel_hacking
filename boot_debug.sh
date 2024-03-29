#qemu-system-x86_64 -m 2G -s -hda ~/operating_systems/linux_distros/Lubuntu/lubuntu.img -enable-kvm -kernel ~/busybox/build/linux-x86-basic/arch/x86_64/boot/bzImage -serial stdio -append "root=/dev/sda1 quiet 3"
BRANCH=`cat branch-linux`

if [ ! -z $1 ]
then
    BRANCH=$1
fi

#from hardrive - normal run
qemu-system-i386 \
    -m 2G -s -hda ~/operating_systems/linux_distros/Lubuntu/lubuntu_32.img \
    -kernel ~/busybox/build/linux-$BRANCH/arch/i386/boot/bzImage \
    -serial stdio \
    -append "root=/dev/sda1 console=ttyS0 nokaslr" 

