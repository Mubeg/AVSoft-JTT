#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mubeg");
MODULE_DESCRIPTION("A loadable Linux module as AVSoft Test task.");
MODULE_VERSION("0.01");

static int __init test_task_init(void) {
 printk(KERN_INFO "Hello, World!\n");
 return 0;
}
static void __exit test_task_exit(void) {
 printk(KERN_INFO "Goodbye, World!\n");
}

module_init(test_task_init);
module_exit(test_task_exit);