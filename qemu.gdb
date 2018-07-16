set disassembly-flavor intel
set auto-load safe-path /
target remote localhost:1234
b /home/geri/workspace/linux/arch/x86/entry/common.c:320 if nr == 223
