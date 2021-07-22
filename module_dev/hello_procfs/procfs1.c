#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/printk.h>
#include <linux/syscalls.h>
#include <asm/cpufeature.h>

#define PROCFS_NAME "helloworld"

// Holds information about the /proc file
struct proc_dir_entry *Our_Proc_File;

#define WRITE_BUFFER_SIZE 4096
unsigned char write_buffer[WRITE_BUFFER_SIZE];

/* Put data into the proc fs file.
*
* Arguments
* =========
* 1. The buffer where the data is to be inserted, if
*
 you decide to use it.
* 2. A pointer to a pointer to characters. This is
*
 useful if you don't want to use the buffer
*
 allocated by the kernel.
* 3. The current position in the file
* 4. The size of the buffer in the first argument.
* 5. Write a "1" here to indicate EOF.
* 6. A pointer to data (useful in case one common
*
 read for multiple /proc/... entries)
*
* Usage and Return Value
* ======================
* A return value of zero means you have no further
* information at this time (end of file). A negative
* return value is an error condition.
*
* For More Information
* ====================
* The way I discovered what to do with this function
* wasn't by reading documentation, but by reading the
* code which used it. I just looked to see what uses
* the get_info field of proc_dir_entry struct (I used a
* combination of find and grep, if you're interested),
* and I saw that it is used in <kernel source
* directory>/fs/proc/array.c.
*
* If something is unknown about the kernel, this is
* usually the way to go. In Linux we have the great
* advantage of having the kernel source code for
* free − use it.
*/



static ssize_t file_read(struct file *filp, 
                        char __user *buff, 
                        size_t length, 
                        loff_t *offset)
{
    unsigned long ret;
    printk(KERN_INFO "procfile_read (/proc/%s) called with buff len: %d\n", 
        PROCFS_NAME, length);
    /*
    * We give all of our information in one go, so if the
    * user asks us if we have more information the
    * answer should always be no.
    *
    * This is important because the standard read
    * function from the library would continue to issue
    * the read system call until the kernel replies
    * that it has no more information, or until its
    * buffer is filled.
    */
    const char* msg = "Hello World!\n";
    if(offset && *offset > 0) {
        // we have finished to read, return 0
        printk(KERN_INFO "offset: %d\n", *offset);
        return 0;
    }
    else {
        // fill the buffer, return the buffer size
        // we could also use put_user to copy from kernel to user space
        // or copy_to_user
        printk("copy_to_user calling...\n");
        // ret = snprintf(buff, length, msg);
        if(copy_to_user(buff, msg, strlen(msg) + 1) != 0)
            ret = 0;
        else
            ret = strlen(msg) + 1;
    }

    printk(KERN_INFO "procfile_read returning: %d\n", ret);

    return ret;

}


static ssize_t file_write(struct file *filp, 
                        const char __user *buff, 
                        size_t length, 
                        loff_t *offset)
{
    ssize_t ret = 0;
    size_t wrlen = length > WRITE_BUFFER_SIZE ? WRITE_BUFFER_SIZE : length;
    if((ret = copy_from_user(write_buffer, buff + *offset, wrlen)) == 0) {
        printk(KERN_INFO "Buffer contents: %s\n", write_buffer);
        return wrlen;
    }
    else {
        printk(KERN_ERR "Failed copy_from_user with err: %lu\n", ret);
        return -ENOMEM;
    }
}

static struct file_operations file = {
    .read = file_read,
    .write = file_write
};

int init_module()
{
    if(boot_cpu_has(X86_FEATURE_SMAP)) {
        printk(KERN_INFO "X86_FEATURE_SMAP is enabled");
    }
    Our_Proc_File = proc_create(PROCFS_NAME, 0644, NULL, &file);
    if (Our_Proc_File == NULL) {
        remove_proc_entry(PROCFS_NAME, NULL);
        printk(KERN_ALERT "Error: Could not initialize /proc/%s\n", PROCFS_NAME);
        return -ENOMEM;
    }
    printk(KERN_INFO "/proc/%s created\n", PROCFS_NAME);
    return 0;
}

void cleanup_module()
{
    remove_proc_entry(PROCFS_NAME, NULL);
    printk(KERN_INFO "/proc/%s removed\n", PROCFS_NAME);
}
