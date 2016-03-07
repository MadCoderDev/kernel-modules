/* headers */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

/* functions */
/* module init */
static int __init m_init(void)
{
	printk(KERN_INFO "Hello World!\n");

	return 0;
}

/* module_exit */
static void __exit m_exit(void)
{ printk(KERN_INFO "See you next time!\n"); }

/***************/
module_init(m_init);
module_exit(m_exit);