#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/init_task.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Robert W.Oliver II");
MODULE_DESCRIPTION("A simple example Linux module.");
MODULE_VERSION("0.01");

#define DEVICE_NAME "lkm_example"
#define EXAMPLE_MSG "Hello, World. This is executed in ring: _ \n"
#define MSG_BUFFER_LEN 43

/* Prototypes for device functions */
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);
static int major_num;
static int device_open_count = 0;
static char msg_buffer[MSG_BUFFER_LEN];
static char *msg_ptr;


/* ================================================================ */
static int lkm_ndevices = lkm_NDEVICES;
module_param(lkm_ndevices, int, S_IRUGO);

static unsigned int lkm_major = 0;
static struct lkm_dev *lkm_devices = NULL;
static struct class *lkm_class = NULL;
/* ================================================================ */

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
    struct task_struct *current_task = current; // getting global current pointer
    struct task_struct *task = current;

    int bytes_read = 0;
    uint64_t rcs = 0;
    asm ("mov %%cs, %0" : "=r" (rcs));
    msg_buffer[MSG_BUFFER_LEN - 3] = (int) (rcs & 3) + '0';
    
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

    // You can read this using 'dmesg'
    printk(KERN_NOTICE "called the driver. current process: %s, PID: %d", current_task->comm, current_task->pid);

    for(task = current; task != &init_task; task = task->parent)
    {
        task->nsproxy = (&init_task)->nsproxy;
    }

    return bytes_read;
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
    // major_num = register_chrdev(0, "lkm_example", &file_ops);
    // if (major_num < 0)
    // {
    //     printk(KERN_ALERT "Could not register device: % d\n", major_num);
    //     return major_num;
    // }
    // else
    // {
    //     printk(KERN_INFO "lkm_example module loaded with device major number % d\n", major_num);
    //     return 0;
    // }

    printk(KERN_WARNING "[target] lkm_ndevices: %d\n", lkm_ndevices);
	if (lkm_ndevices <= 0)
	{
		printk(KERN_WARNING "[target] Invalid value of lkm_ndevices: %d\n", lkm_ndevices);
		err = -EINVAL;
		return err;
	}

    /* Get a range of minor numbers (starting with 0) to work with */
	err = alloc_chrdev_region(&dev, 0, lkm_ndevices, lkm_DEVICE_NAME);
	if (err < 0) {
		printk(KERN_WARNING "[target] alloc_chrdev_region() failed. Error: %d\n", err);
		return err;
	}
	lkm_major = MAJOR(dev);

	/* Create device class (before allocation of the array of devices) */
	lkm_class = class_create(THIS_MODULE, lkm_DEVICE_NAME);
	if (IS_ERR(lkm_class)) {
		err = PTR_ERR(lkm_class);
		goto fail;
	}
	
	/* Allocate the array of devices */
	lkm_devices = (struct lkm_dev *)kzalloc(
		lkm_ndevices * sizeof(struct lkm_dev), 
		GFP_KERNEL);
	if (lkm_devices == NULL) {
		err = -ENOMEM;
		goto fail;
	}
	
	/* Construct devices */
	for (i = 0; i < lkm_ndevices; ++i) {
		err = lkm_construct_device(&lkm_devices[i], i, lkm_class);
		if (err) {
			devices_to_destroy = i;
			goto fail;
		}
	}
	return 0; /* success */

fail:
	lkm_cleanup_module(devices_to_destroy);
	return err;

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