qemu-system-x86_64 \
  -kernel ~/busybox/build/linux-master/arch/x86_64/boot/bzImage \
  -initrd ~/busybox/build/initramfs-busybox-x86.cpio.gz \
  -nographic -append "console=ttyS0 nokaslr" \
  -s
