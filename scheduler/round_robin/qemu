qemu-system-x86_64 -s -hda /dev/zero -kernel arch/x86/boot/bzImage -serial stdio -append "boot_delay=2"

qemu-system-x86_64 -m 2G -s -hda ../linux.img -kernel arch/x86/boot/bzImage -serial stdio -append "root=/dev/sda1"

http://www.yonch.com/tech/84-debugging-the-linux-kernel-with-qemu-and-eclipse

http://linux-tips.org/t/booting-from-an-iso-image-using-qemu/136



Qemu booting with ISO image
===========================

Qemu has two operating mode named full system emulation and user mode emulation. If you want to simulate whole system not just the cpu (like a PC) you need to use full system emulation mode.

Newer distributions have separate binaries for these two different operation modes. For example, if you just want user mode cpu emulation for X86-64 architecture, you need to use qemu-x86_64 binary and if you need whole X86-64 bit system emulation (like your PC), qemu-system-x86_64 binary must be used in commands below.

You have an iso image and you want to boot from it without restarting the system, simply use qemu virtual machine as below (-m 512 says qemu will be use 512 Mb of RAM from system):

qemu-system-x86_64 -boot d -cdrom image.iso -m 512

It is also possible to use your regular cdrom device too. If the device is /dev/cdrom you can boot a cd in the device like that:

qemu-system-x86_64 -boot d -cdrom /dev/cdrom -m 512

Above examples not use any harddisk, so it is suitable for live cd image case. If you want to install a distribution to a harddisk image file, you need to create harddisk image file first:

qemu-img create mydisk.img 10G

In this example we’re created an image of 10 GB. Now we can use this file as harddisk in our qemu boot:

qemu-system-x86_64 -boot d -cdrom image.iso -m 512 -hda mydisk.img


Qemu and Eclipse debugging
==========================

Debugging the Linux kernel with Qemu and Eclipse
Posted on August 12, 2013 by yonch
First, it is useful to have Eclipse index all kernel symbols, and an excellent tutorial can be found here.

Older tutorials on setting up kernel debugging with QEMU and Eclipse were extremely useful in getting the setup working, however more recent changes to QEMU requires some effort to bypass limitations in gdb, and Eclipse dialogs have changed enough to warrent an updated tutorial. These tutorials include Takis blog and Linuxforu.

Configure the GNU/Linux kernel
The Ubuntu 12.04 Desktop and Server kernels come already configured with CONFIG_DEBUG_INFO and CONFIG_FRAME_POINTER, that seem to be required for gdb kernel debugging. It is an established practice for distributions to put their kernel configurations in /boot/, in files named config-<kernel_version>, so checking an existing configuration should be straightforward with grep.

To run a custom-compiled kernel in QEMU, Realtek 8139 C+ Ethernet controller support should be compiled into the kernel: CONFIG_8139CP and CONFIG_8139TOO. Usually these are compiled as modules, however compiling directly to the kernel allows booting the kernel using QEMU without worrying about modules. To configure the kernel, go to Device Drivers -> Network device support -> Ethernet driver support -> Realtek devices, and choose both “RTL-8139 C+” and “RTL-8129/8130/8139”. It is easier to do with “make xconfig” than with “make menuconfig” because xconfig sorts list entries.

Disabling soft lockup and rcu_sched stall warnings
You might experience console output like “INFO: rcu_sched detected stall on CPU 0 (t=40628 jiffies)”. This is caused by the Linux kernel’s RCU CPU Stall Detector. It appears to be a good idea to keep the warnings, since it could help hint at bugs that affect scheduling, however if these warnings get tiresome, the warning interval can be increased via the CONFIG_RCU_CPU_STALL_TIMEOUT configuration parameter (Kernel Hacking -> RCU CPU stall timeout in seconds).

The kernel has a watchdog mechanism to warn in case a kernel tasklet/interrupt handler/system call code hogs the CPU for too long. Output could look like “BUG: soft lockup – CPU#0 stuck for 80s!” and a call trace. This feature can be controlled using the CONFIG_LOCKUP_DETECTOR (Kernel hacking -> Kernel Debugging -> Detect Hard and Soft Lockups).

Build the kernel
If modules are of no interest, a ‘make bzImage’ might be sufficient. Otherwise, in Ubuntu there is a relatively easy way of packaging modules with the kernel (the “alternative build method“), for installation into the disk image created below:


make -j16 deb-pkg 
1
make -j16 deb-pkg 
Be ready for this to take several GB of storage. The result is a dpkg file

Create a disk image
To set up a linux disk image, one uses the qemu-img command. I created a 1 GB raw disk:


qemu-img create -f raw linux.img 1G 
1
qemu-img create -f raw linux.img 1G 
Install a distrubution of your choice. Ubuntu Server 12.04 (a Long Term Support version) installed with some 90 MB to spare. The installation was quite fast with multiple SMP cores (via the SMP parameter).


qemu-system-x86_64 -m 1G -hda linux.img -cdrom ~/Downloads/ubuntu-12.04.2-server-amd64.iso -boot d -smp 8
1
qemu-system-x86_64 -m 1G -hda linux.img -cdrom ~/Downloads/ubuntu-12.04.2-server-amd64.iso -boot d -smp 8
An alternative method of building the disk image using buildroot is proposed in a linux-magazine tutorial.

Editing the disk image
In case you need to change the image, for example install newly compiled modules, it’s possible to use kpartx to setup /dev/mapper/…, and then mount that directory. As root:


sudo apt-get install kpartx sudo kpartx -av linux.img    # output was "add map loop0p1 (252:0): 0 3141632 linear /dev/loop0 2048" mkdir img-mnt sudo mount /dev/mapper/loop0p1 img-mnt/
1
sudo apt-get install kpartx sudo kpartx -av linux.img    # output was "add map loop0p1 (252:0): 0 3141632 linear /dev/loop0 2048" mkdir img-mnt sudo mount /dev/mapper/loop0p1 img-mnt/
 Cleaning up:


sudo umount img-mnt sudo kpartx -d linux.img  # output: "loop deleted : /dev/loop0" 
1
sudo umount img-mnt sudo kpartx -d linux.img  # output: "loop deleted : /dev/loop0" 
Run the custom kernel with the disk image
To run the kernel in QEMU


qemu-system-x86_64 -m 1G -hda linux.img  -kernel arch/x86_64/boot/bzImage -append root=/dev/sda1 -s 
1
qemu-system-x86_64 -m 1G -hda linux.img  -kernel arch/x86_64/boot/bzImage -append root=/dev/sda1 -s 
-m 1G: 1 GB of RAM
-append root=/dev/sda1: the kernel command line parameters. This boots out of linux.img’s first partition
-s: run a gdbserver
Consider redirecting the console to the shell running the qemu command:


qemu-system-x86_64 -m 1G -hda linux.img  -kernel arch/x86_64/boot/bzImage -append "root=/dev/sda1 console=ttyS0" -s -serial stdio 
1
qemu-system-x86_64 -m 1G -hda linux.img  -kernel arch/x86_64/boot/bzImage -append "root=/dev/sda1 console=ttyS0" -s -serial stdio 
It might be even more convenient to forgo the graphic shell and open the login console on the same terminal running qemu. This requires configuring a serial console in the guest system: in Ubuntu, this entails creating a configuration file in /etc/init; see the Ubuntu Serial Console Howto. On other systems, a similar result can be accomplished by editing /etc/inittab. The command is now


qemu-system-x86_64 -m 1G -hda linux.img  -kernel arch/x86_64/boot/bzImage -append "root=/dev/sda1 console=ttyS0" -s -nographic 
1
qemu-system-x86_64 -m 1G -hda linux.img  -kernel arch/x86_64/boot/bzImage -append "root=/dev/sda1 console=ttyS0" -s -nographic 
Note that other tutorials also add a “-S” parameter so QEMU starts the kernel stopped, however this is ommitted deliberately. The “-S” parameter would allow gdb to set an initial breakpoint anywhere in the kernel before kernel execution begins. Unfortunately, a change made to the gdbserver in QEMU, to support debugging 32- and 16-bit guest code in an x86_64 session breaks the -S functionality. The symptoms are that gdb prints out “Remote ‘g’ packet reply is too long:”, and fails to interact successfully with QEMU. The suggested fix is to run the QEMU until it is in 64-bit code (i.e. after the boot loader has finished and the kernel started) before connecting from gdb (omitting the -S parameter). To debug a running kernel, this is sufficient; it is the method we will take.

Debug the kernel from the command line
The kernel compilation process produces a compressed image file, bzImage, and an uncompressed object file, vmlinux. Whereas QEMU runs bzImage, the debugger needs vmlinux, which generally is produced in the root of the source distribution. To debug, run


gdb vmlinux 
1
gdb vmlinux 
After your kernel has booted, attach to it from gdb using the target command. The default qemu debug port is 1234.


(gdb) target remote localhost:1234 Remote debugging using localhost:1234 default_idle () at arch/x86/kernel/process.c:314 314		trace_cpu_idle_rcuidle(PWR_EVENT_EXIT, smp_processor_id()); 
1
(gdb) target remote localhost:1234 Remote debugging using localhost:1234 default_idle () at arch/x86/kernel/process.c:314 314		trace_cpu_idle_rcuidle(PWR_EVENT_EXIT, smp_processor_id()); 
The rest should feel like a regular gdb session. For example:


(gdb) b qdisc_create Breakpoint 1 at 0xffffffff815c1db0: file net/sched/sch_api.c, line 870. (gdb) c Continuing.  Breakpoint 1, qdisc_create (dev=&lt;optimized out&gt;, dev_queue=&lt;optimized out&gt;,      p=&lt;optimized out&gt;, parent=&lt;optimized out&gt;, handle=&lt;optimized out&gt;,      tca=&lt;optimized out&gt;, errp=0xffff8800382cda74) at net/sched/sch_api.c:870 870	{ (gdb) c Continuing. 
1
(gdb) b qdisc_create Breakpoint 1 at 0xffffffff815c1db0: file net/sched/sch_api.c, line 870. (gdb) c Continuing.  Breakpoint 1, qdisc_create (dev=&lt;optimized out&gt;, dev_queue=&lt;optimized out&gt;,      p=&lt;optimized out&gt;, parent=&lt;optimized out&gt;, handle=&lt;optimized out&gt;,      tca=&lt;optimized out&gt;, errp=0xffff8800382cda74) at net/sched/sch_api.c:870 870	{ (gdb) c Continuing. 
Some have noted it might be useful to compile the kernel with less optimization and to include frame pointers, but I haven’t experimented with these.

Debug the kernel from Eclipse
This should work in Eclipse Kepler.

Go to Run -> Debug Configurations..
Add a C/C++ Attach to Application configuration.
In the “Main” tab, under C/C++ Application, put the path to vmlinux, e.g., /home/myuser/linux-build/vmlinux. Optionally disable autobuild if reasonable.
The bottom of the “Main” tab should say something like “Using GDB (DSF) Automatic Remote Debugging Launcher”. Click “Select other…” and choose “GDB (DSF) Manual Remote Debugging Launcher”.
In the “Debugger” tab, clear the tickbox next to “Stop on startup at:” (because gdb is unable to debug the QEMU bootloader)
The “Connection” sub-tab of the “Debugger” tab should be configured to TCP, localhost, and port 1234.
Now it should be possible to go to attach to QEMU from the Debug Configurations menu (or after the first launch, from the toolbar bug icon).

Debug a module
To get debug symbols in a module, it should be compiled with CONFIG_DEBUG_INFO. When building the kernel, we turned this configuration parameter on, so compiling against the headers from the custom kernel will do the trick. Note that the headers in /lib/modules/<kernel_version>/build might have the CONFIG_DEBUG_INFO=n even though the configuration in /boot/config-<kernel_version> has CONFIG_DEBUG_INFO=y (at least in Ubuntu 12.04), so even if you hadn’t made changes to the /boot/config version, compile against the headers in your custom kernel, not the ones in /lib/.., to get debug symbols in the module.

To debug a kernel module, gdb has to read the object file, and be given the object’s location in memory. The kernel exposes the memory mapping in /sys/module/<module_name>/sections/. Get the addresses by reading the .text, .data and .bss, then update gdb by issuing


add-symbol-file /path/to/module &lt;.text_addr&gt; -s .data &lt;.data_addr&gt; -s .bss &lt;.bss_addr&gt;
1
add-symbol-file /path/to/module &lt;.text_addr&gt; -s .data &lt;.data_addr&gt; -s .bss &lt;.bss_addr&gt;
In Eclipse, this command can be entered into the gdb in Console View, just make sure to suspend the kernel’s execution (Run->Suspend or the yellow “pause” button in the toolbar) before writing into the console, or gdb wouldn’t get the input. Eclipse can then set breakpoints in the module’s code.

Debugging kernel OOPS
Given the oops message, it’s possible to get quite far in finding the problem. However, with gdb attached to the kernel, we can do better! The secret sauce is catching the oops in gdb, which can be done by setting a breakpoint in the oops handler:


(gdb) b panic Breakpoint 1 at 0xffffffff8168fd18: file kernel/panic.c, line 70. (gdb) c Continuing.  Breakpoint 1, panic (fmt=&lt;optimized out&gt;) at kernel/panic.c:70 70	{ 
1
(gdb) b panic Breakpoint 1 at 0xffffffff8168fd18: file kernel/panic.c, line 70. (gdb) c Continuing.  Breakpoint 1, panic (fmt=&lt;optimized out&gt;) at kernel/panic.c:70 70	{ 
Now, the offending function will be in the backtrace. For example:


(gdb) backtrace  #0  panic (fmt=&lt;optimized out&gt;) at kernel/panic.c:70 #1  0xffffffff8169c71a in oops_end (flags=582, regs=0xffff88003fc03d38,      signr=9) at arch/x86/kernel/dumpstack.c:240 &lt; SNIPPED SOME LINES &gt; #6  0xffffffff8169f3de in do_page_fault (regs=&lt;optimized out&gt;,      error_code=&lt;optimized out&gt;) at arch/x86/mm/fault.c:1231 #7  &lt;signal handler called&gt; #8  bstats_update (bstats=0xffff88003cd1f8a0, skb=0x0)     at include/net/sch_generic.h:483 #9  qdisc_bstats_update (skb=0x0, sch=0xffff88003cd1f800)     at include/net/sch_generic.h:490 #10 fastpass_dequeue (sch=0xffff88003cd1f800) at net/sched/sch_fastpass.c:519 #11 0xffffffff815be58e in dequeue_skb (q=0xffff88003cd1f800) &lt; MORE SNIPPED &gt; 
1
(gdb) backtrace  #0  panic (fmt=&lt;optimized out&gt;) at kernel/panic.c:70 #1  0xffffffff8169c71a in oops_end (flags=582, regs=0xffff88003fc03d38,      signr=9) at arch/x86/kernel/dumpstack.c:240 &lt; SNIPPED SOME LINES &gt; #6  0xffffffff8169f3de in do_page_fault (regs=&lt;optimized out&gt;,      error_code=&lt;optimized out&gt;) at arch/x86/mm/fault.c:1231 #7  &lt;signal handler called&gt; #8  bstats_update (bstats=0xffff88003cd1f8a0, skb=0x0)     at include/net/sch_generic.h:483 #9  qdisc_bstats_update (skb=0x0, sch=0xffff88003cd1f800)     at include/net/sch_generic.h:490 #10 fastpass_dequeue (sch=0xffff88003cd1f800) at net/sched/sch_fastpass.c:519 #11 0xffffffff815be58e in dequeue_skb (q=0xffff88003cd1f800) &lt; MORE SNIPPED &gt; 
The backtrace enables exploring the code that led to the panic; we can examine individual frames, for example issuing “frame 10”, listing code with repeated calls to “list” or “list -“, and printing variables with “print varname”.

