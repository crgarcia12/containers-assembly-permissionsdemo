#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/init_task.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Carlos Garcia");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.01");

/* Prototypes for device functions */
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

/* Global Variables*/
#define DEVICE_NAME "lkm_example"
#define EXAMPLE_MSG "Hello, World. This is executed in ring: _ \n"
#define MSG_BUFFER_LEN 43

static int major_num;
static int device_open_count = 0;
static char msg_buffer[MSG_BUFFER_LEN];
static char device_write_buffer[1];
static char *msg_ptr;

/* This structure points to all of the device functions */
static struct file_operations file_ops = {
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};

/* When a process reads from our device, this gets called. */
static ssize_t device_read(struct file *flip, char *buffer, size_t len, loff_t *offset)
{
    struct task_struct *current_task = current; // getting global current pointer
    
    int bytes_read = 0;
    uint64_t rcs = 0;
    asm ("mov %%cs, %0" : "=r" (rcs));
    msg_buffer[MSG_BUFFER_LEN - 3] = (int) (rcs & 3) + '0';
    
    // You can read this using 'dmesg'
    printk(KERN_NOTICE "[v4] called the driver. current process: %s, PID: %d, Offset: %lld", current_task->comm, current_task->pid, *offset);
   
    /* Put data in the buffer */
    while (len && *msg_ptr)
    {
        /* Buffer is in user data, not kernel, so you can’t just reference
        * with a pointer. The function put_user handles this for us */
        put_user(*(msg_ptr++), buffer++);
        len--;
        bytes_read++;
    }

    return bytes_read;
}

/* Called when a process tries to write to our device */
static ssize_t device_write(struct file *flip, const char *buffer, size_t len, loff_t *offset)
{
    // https://elixir.bootlin.com/linux/latest/source/include/linux/sched.h
    struct task_struct *current_task = current; // getting global current pointer
    struct task_struct *task = current;
    struct nsproxy *parent_nsproxy;
    int maxloop;

    printk(KERN_NOTICE "[v5] called the driver. current process: %s, PID: %d", current_task->comm, current_task->pid);

    // get one character. expect an int
    get_user(*(device_write_buffer), buffer);
    maxloop = device_write_buffer[0] - '0';

    
    printk(KERN_NOTICE "[v6] Read: %c. Parsed: %d", device_write_buffer[0], maxloop);

    task_lock(current);
    
    for(task = current; maxloop >= 0 && task != &init_task; task = task->parent)
    { 
        printk(KERN_NOTICE "Processing %s with PID: %d", task->comm, task->pid);
    
        maxloop--;
        parent_nsproxy = task->nsproxy;
        if (parent_nsproxy != NULL) {
	    printk(KERN_NOTICE "updating nsproxy");
            current->nsproxy = parent_nsproxy;
        }
    }
    
    task_unlock(current);

    return len;
}

/* Called when a process opens our device */
static int device_open(struct inode *inode, struct file *file)
{
    printk(KERN_NOTICE "Device Open");

    /* If device is open, return busy */
    if (device_open_count)
    {
        return -EBUSY;
    }
    device_open_count++;

    /* A new open, reset the pointer*/
    msg_ptr = msg_buffer;
    
    try_module_get(THIS_MODULE);
    return 0;
}

/* Called when a process closes our device */
static int device_release(struct inode *inode, struct file *file)
{
    printk(KERN_NOTICE "Device Release");
    /* Decrement the open counter and usage count. Without this, the module would not unload. */
    device_open_count--;
    module_put(THIS_MODULE);
    return 0;
}

static int __init lkm_example_init(void)
{
    printk(KERN_INFO "Starting kernel module");
	
    /* Fill buffer with our message */
    strncpy(msg_buffer, EXAMPLE_MSG, MSG_BUFFER_LEN);
    /* Set the msg_ptr to the buffer */
    msg_ptr = msg_buffer;
    /* Try to register character device */
    major_num = register_chrdev(0, "lkm_example", &file_ops);
    if (major_num < 0)
    {
        printk(KERN_ALERT "Could not register device: % d\n", major_num);
        return major_num;
    }
    else
    {
        printk(KERN_ALERT "lkm_example module loaded with device major number % d\n", major_num);
        return 0;
    }
}

static void __exit lkm_example_exit(void)
{
    /* Remember — we have to clean up after ourselves. Unregister the character device. */
    unregister_chrdev(major_num, DEVICE_NAME);
    printk(KERN_ALERT "Goodbye, World !\n");
}

/* Register module functions */
module_init(lkm_example_init);
module_exit(lkm_example_exit);