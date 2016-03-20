/* headers */
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

/* defs */
#define DEV		"chrdev"
#define BUFLEN	80

/* prototypes */
static int	__init chrdev_init(void);
static void __exit chrdev_exit(void);
static int 		dev_open(struct inode*, struct file*);
static int 		dev_close(struct inode*, struct file*);
static ssize_t 	dev_read(struct file*, char*, size_t, loff_t*);
static ssize_t	dev_write(struct file*, const char*, size_t, loff_t*);

/* variables */
static int 		major;
static int 		is_open; 				/* 1 if the device is open */
static char		dev_buffer[BUFLEN];
static char* 	bufferp;				/* buffer pointer */
static struct file_operations fops = {
				.open = dev_open,
				.read = dev_read,
				.write = dev_write,
				.release = dev_close
			};

/* INIT and EXIT */
/* chrdev_init */
static int __init chrdev_init(void)
{
	major = register_chrdev(0, DEV, &fops);
	if(major < 0)
	{
		printk(KERN_ALERT "CHRDEV: register_chrdev() failed, major < 0.\n");
		return major;
	}

	printk(KERN_INFO "CHRDEV: device[%d] registered with name \"%s\"\n", major, DEV);
	return 0;
}	

/* chrdev_exit */
static void __exit chrdev_exit(void)
{
	unregister_chrdev(major, DEV);
}

/* FOPS functions */
/* dev_open */
static int dev_open(struct inode* inode, struct file* fp)
{
	if(is_open == 1)
		return -EBUSY;

	is_open = 1;
	bufferp = dev_buffer;
	try_module_get(THIS_MODULE);

	return 0;
}

/* dev_close */
static int dev_close(struct inode* inode, struct file* fp)
{
	is_open = 0;
	module_put(THIS_MODULE);

	return 0;
}

/* dev_read */
static ssize_t dev_read(struct file* fp, char* buffer, size_t length, loff_t* offset)
{
	int bytes = 0;

	if(*bufferp == 0)
		return 0;

	while(length-- && *bufferp)
	{
		++bytes;
		put_user(*(bufferp++), buffer++);
	}

	return bytes;
}	

/* dev_write */
static ssize_t dev_write(struct file* fp, const char* buffer, size_t length, loff_t* offset)
{
	int i;
	for(i = 0; (i < length) && (i < BUFLEN); i++)
		get_user(dev_buffer[i], buffer++);

	bufferp = dev_buffer;

	return i;
}

/* module_init and module_exit */
module_init(chrdev_init);
module_exit(chrdev_exit);