#qemu-system-x86_64 -m 2G -s -hda ~/operating_systems/linux_distros/Lubuntu/lubuntu.img -enable-kvm -kernel ~/busybox/build/linux-x86-basic/arch/x86_64/boot/bzImage -serial stdio -append "root=/dev/sda1 quiet 3"

#from hardrive - normal run
qemu-system-i386 \
    -m 2G -s -hda ~/operating_systems/linux_distros/Lubuntu/lubuntu_32.img \
    -kernel ~/busybox/build/linux-x86-basic/arch/i386/boot/bzImage \
    -serial stdio \
    -append "root=/dev/sda1 console=ttyS0 nokaslr" 

