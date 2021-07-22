#include <linux/module.h>       /* Needed by all modules, module_init, module_exit */
#include <linux/kernel.h>       /* Needed for KERN_INFO */
#include <linux/init.h>         /* Needed for macros */
#include <linux/fs.h>
#include <asm/uaccess.h>        /* get_user, put_user */
#include <linux/syscalls.h>

#include "char-dev.h"

#define SUCCESS 0
#define DEVICE_NAME "char_dev"   /* /proc/devices/char_dev */
#define BUF_LEN 80              /* Maximum length of message from device */

/* 
 * Is the device open right now? Used to prevent
 * concurrent access to the same device
 */
static int Device_Open = 0;

/*
* The message the device will give when asked
*/
static char Message[BUF_LEN];
/*
* How far did the process reading the message get?
* Useful if the message is larger than the size of the
* buffer we get to fill in device_read.
*/
static char *Message_Ptr;

/*
 * Methods
 */

static int device_open(struct inode *inode, struct file *filp)
{
    printk(KERN_INFO "device open (%p,%p)\n", inode, filp);
    /*
     * We don't want to talk to 2 processes at the same time.
     */
    if(Device_Open)
        return -EBUSY;

    Device_Open++;
    /*
     * Initialize the message
     */
    Message_Ptr = Message;
    try_module_get(THIS_MODULE);
    return SUCCESS;
}

/** Called when a process closes the file
 */
static int device_release(struct inode *inode, struct file *filp)
{
    printk(KERN_INFO "device release (%pK,%pK)\n", inode, filp);
    Device_Open--;      // now we are ready for our next caller
    /*
     * Decrement the usage count, or else once you opened the file, you'll never get
     * rid of the module.
     */
    module_put(THIS_MODULE);
    return SUCCESS;
}

/*
 * Called when a process, which already opened the file, attempts to read from it
 */

static ssize_t device_read(struct file *filp,    // see linux/fs.h
                           char __user *buffer,  // buffer to fill with data
                           size_t length,        // length of buffer
                           loff_t *offset)
{
    /*
     * Number of bytes actually written to the buffer
     */
    int bytes_read = 0;
    printk(KERN_INFO "device_read(%p,%p,%d)\n", filp, buffer, length);
    /*
     * If we're at the end of the message, return 0
     * (which signifies end of file)
     */
    if (*Message_Ptr == 0)
        return 0;
    /*
    * Actually put the data into the buffer
    */
    while (length && *Message_Ptr) {
        /*
        * Because the buffer is in the user data segment,
        * not the kernel data segment, assignment wouldn't
        * work. Instead, we have to use put_user which
        * copies data from the kernel data segment to the
        * user data segment.
        */
        put_user(*(Message_Ptr++), buffer++);
        length--;
        bytes_read++;
    }
    printk(KERN_INFO "Read %d bytes, %d left\n", bytes_read, length);
    /*
     * Read functions are supposed to return the number
     * of bytes actually inserted into the buffer
     */
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
    ssize_t i;
    int e;
    printk(KERN_INFO "device_write(%p,%s,%d)", filp, buffer, length);
    for(i=0;i<length && i < BUF_LEN; i++) {
        if((e = get_user(Message[i], buffer+i)) != 0) {
            printk(KERN_ERR "get_user failed at: %d, %p\n", i, Message + i);
        }
    }
    Message_Ptr = Message;
    /*
     * return number of input characters used
     */
    return i;
}

/*
* This function is called whenever a process tries to do an ioctl on our
* device file. We get two extra parameters (additional to the inode and file
* structures, which all device functions get): the number of the ioctl called
* and the parameter given to the ioctl function.
*
* If the ioctl is write or read/write (meaning output is returned to the
* calling process), the ioctl call returns the output of this function.
*
*/

long device_ioctl(struct file *filp, 
                  unsigned int ioctl_num, 
                  unsigned long ioctl_param)
{
    int i;
    char *temp;
    char ch;

    /*
     * Switch according to ioctl called
     */
    switch (ioctl_num) {
    case IOCTL_SET_MSG:
        /*
         * Receive a pointer to a message (in user space) and set that
         * to be the device's message. Get the parameter given to ioctl
         * by the process.
         */
        temp = (char *)ioctl_param;
        /* 
         * Find the length of the message
         */
        get_user(ch, temp);
        for (i = 0; ch && i < BUF_LEN; i++, temp++) {
            get_user(ch, temp);
        }
        device_write(filp, (char*)ioctl_param, i, 0);
        break;
    case IOCTL_GET_MSG:
        /*
         * Give the current message to the calling process −
         * the parameter we got is a pointer, fill it.
         */
        i = device_read(filp, (char *)ioctl_param, 99, 0);
        /*
        * Put a zero at the end of the buffer, so it will be
        * properly terminated
        */
        put_user('\0', (char *)ioctl_param + i);
        break;
    case IOCTL_GET_NTH_BYTE:
        /*
        * This ioctl is both input (ioctl_param) and
        * output (the return value of this function)
        */
        return Message[ioctl_param];
        break;
    }
    return SUCCESS;
}


static struct file_operations fops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release,
    .unlocked_ioctl = device_ioctl
};


static int __init initmodule(void ){
    int ret_val;
    ret_val = register_chrdev(MAJOR_NUM, DEVICE_NAME, &fops);
    /*
    * Negative values signify an error
    */
    if (ret_val < 0) {
        printk(KERN_ALERT "%s failed with %d\n",
            "Sorry, registering the character device ", ret_val);
        return ret_val;
    }

    printk(KERN_INFO "%s The major device number is %d.\n",
        "Registeration is a success", MAJOR_NUM);
    printk(KERN_INFO "If you want to talk to the device driver,\n");
    printk(KERN_INFO "you'll have to create a device file. \n");
    printk(KERN_INFO "We suggest you use:\n");
    printk(KERN_INFO "mknod /dev/%s c %d 0\n", DEVICE_FILE_NAME, MAJOR_NUM);
    printk(KERN_INFO "The device file name is important, because\n");
    printk(KERN_INFO "the ioctl program assumes that's the\n");
    printk(KERN_INFO "file you'll use.\n");


    return SUCCESS;       // must return 0 on success and nonzero on failure
}

static void __exit exitmodule(void ){
    int ret;
    /*
    * Unregister the device
    */
    unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
}

module_init( initmodule );
module_exit( exitmodule );

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Gergely Bod");
MODULE_DESCRIPTION("A Char Device Module");

