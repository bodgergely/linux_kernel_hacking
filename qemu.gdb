set disassembly-flavor intel
set auto-load safe-path /
target remote localhost:1234
lx-symbols
apropos lx
#b do_boot_cpu
#b start_kernel
