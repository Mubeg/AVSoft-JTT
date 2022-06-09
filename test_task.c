#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mubeg");
MODULE_DESCRIPTION("A loadable Linux module as AVSoft Test task.");
MODULE_VERSION("0.02");

static struct proc_dir_entry *proc;

static ssize_t mywrite(struct file *file, const char __user *ubuf,size_t count, loff_t *ppos) {

    printk( KERN_DEBUG "write handler\n");

    return -1;
}
 
static ssize_t myread(struct file *file, char __user *ubuf,size_t count, loff_t *ppos) {

    printk( KERN_DEBUG "read handler\n");

    return 0;
}
 
static struct file_operations myops = {

    .owner = THIS_MODULE,
    .read = myread,
    .write = mywrite,

};

static int __init test_task_init(void) {

    proc = proc_create("test_task",0666,NULL,&myops);

    return 0;
}
static void __exit test_task_exit(void) {

    proc_remove(proc);
}

module_init(test_task_init);
module_exit(test_task_exit);