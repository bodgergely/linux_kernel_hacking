#include <linux/module.h>       /* Needed by all modules, module_init, module_exit */
#include <linux/kernel.h>       /* Needed for KERN_INFO */
#include <linux/init.h>         /* Needed for macros */
// #include <linux/slab.h>
// #include <linux/vmalloc.h>
// #include <linux/mm.h>
// #include <asm/unistd.h>
// #include <asm/page.h>
#include <linux/syscalls.h>     /* __init and __exit */
#include <linux/printk.h>


static void greeting(void)
{
    printk(KERN_INFO "Hello Marcika!\n");    
}

static void goodbye(void)
{
    printk(KERN_INFO "Goodbye from Hello World module!\n");
}

static int __init initmodule(void ){
    greeting();
    return 0;       // must return 0 on success and nonzero on failure
}

static void __exit exitmodule(void ){
    goodbye();
	return;
}

module_init( initmodule );
module_exit( exitmodule );

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Gergely Bod");
MODULE_DESCRIPTION("A Hello World Module");

