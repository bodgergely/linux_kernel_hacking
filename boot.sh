qemu-system-i386 \
  -kernel ~/busybox/build/linux-x86-basic/arch/i386/boot/bzImage \
  -initrd ~/busybox/build/initramfs-busybox-x86.cpio.gz \
  -nographic -append "console=ttyS0" \
  -enable-kvm \
  -s
