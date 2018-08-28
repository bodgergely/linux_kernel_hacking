qemu-system-i386 \
  -kernel ~/busybox/build/linux-master/arch/i386/boot/bzImage \
  -initrd ~/busybox/build/initramfs-busybox-x86.cpio.gz \
  -nographic -append "console=ttyS0 nokaslr" \
  -enable-kvm \
  -s
