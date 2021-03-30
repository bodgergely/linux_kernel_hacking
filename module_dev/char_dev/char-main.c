#include <linux/module.h>       /* Needed by all modules, module_init, module_exit */
#include <linux/kernel.h>       /* Needed for KERN_INFO */
#include <linux/init.h>         /* Needed for macros */
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/syscalls.h>


/****************************************************************
* Prototypes
*/
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char __user *, size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "char_dev"   /* /proc/devices/char_dev */
#define BUF_LEN 80              /* Maximum length of message from device */

static int Major;
static int Device_Open = 0;

static char msg[BUF_LEN];
static char *msg_Ptr;


static struct file_operations fops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};

/*
 * Methods
 */

static int device_open(struct inode *inode, struct file *filp)
{
    static int counter = 0;

    if(Device_Open)
        return -EBUSY;

    Device_Open++;
    sprintf(msg, "I already told you %d times Hello World!\n", counter++);
    msg_Ptr = msg;
    try_module_get(THIS_MODULE);
    return SUCCESS;
}

/** Called when a process closes the file
 */
static int device_release(struct inode *inode, struct file *filp)
{
    Device_Open--;      // now we are ready for our next caller
    /*
     * Decrement the usage count, or else once you opened the file, you'll never get
     * rid of the module.
     */
    module_put(THIS_MODULE);
    return 0;
}

/*
 * Called when a process, which already opened the file, attempts to read from it
 */

static ssize_t device_read(struct file *filp,    // see linux/fs.h
                           char __user *buffer,  // buffer to fill with data
                           size_t length,        // length of buffer
                           loff_t *offset)
{
    // number of bytes written into the buffer
    int bytes_read = 0;

    // if we are at the end of the message, return 0 signifying the end of file
    if(*msg_Ptr == '\0')
        return 0;
    
    // Put the data into the buffer
    while(length && *msg_Ptr) {
        // buffer is in user data segment, so we can't apply operator *,
        // we have to use put_user which copies from kernel memory to
        // user memory segment
        put_user(*(msg_Ptr++), buffer++);
        length--;
        bytes_read++;
    }
    // most read functions return the number of bytes put into buffer
    return bytes_read;
}

/*
 * Called when process writes to device file: echo -n "hi" > /dev/char_dev
 */

static ssize_t device_write(struct file *filp,    // see linux/fs.h
                           const char __user *buffer,   // buffer to write from
                           size_t length,        // length of buffer
                           loff_t *offset)
{
    printk(KERN_ALERT "Sorry this operation is not supported!\n");
    return -EINVAL;
}


static int __init initmodule(void ){
    Major = register_chrdev(0, DEVICE_NAME, &fops);
    if (Major < 0) {
        printk(KERN_ALERT "Registering char device failed with %d\n", Major);
        return Major;
    }
    printk(KERN_INFO "I was assigned major number: %d\n", Major);
    printk(KERN_INFO "To talk to the driver create a dev file with\n");
    printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, Major);
    printk(KERN_INFO "Try various minor numbers. Try to cat and echo to\n");
    printk(KERN_INFO "the device file.\n");
    printk(KERN_INFO "Remove the device file and module when done.\n");

    return SUCCESS;       // must return 0 on success and nonzero on failure
}

static void __exit exitmodule(void ){
    /* 
    * Unregister device
    */
    unregister_chrdev(Major, DEVICE_NAME);
	return;
}

module_init( initmodule );
module_exit( exitmodule );

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Gergely Bod");
MODULE_DESCRIPTION("A Char Device Module");

