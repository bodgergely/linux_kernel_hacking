#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/mm.h>
#include <asm/unistd.h>
#include <asm/page.h>
#include <linux/syscalls.h>
#include <linux/printk.h>


static void printk_test(void)
{
    printk(KERN_INFO "Hello World!\n");    
}

static void goodbye(void)
{
    printk(KERN_INFO "Goodbye from Hello World module!\n");
}

static int __init initmodule(void ){
    printk_test();
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

