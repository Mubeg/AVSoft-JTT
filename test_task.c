#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mubeg");
MODULE_DESCRIPTION("A loadable Linux module as AVSoft Test task.");
MODULE_VERSION("0.03");

#define MAX_BUFF_SIZE 100

static char buff[MAX_BUFF_SIZE] = {};

static struct proc_dir_entry *proc_entry = NULL;

static ssize_t write(struct file *file, const char __user *user_buff, size_t count, loff_t *offset) {

    int len = strlen(buff);
    printk(KERN_DEBUG "\nBuff before = {\n%s}\n", buff);

    if(count > MAX_BUFF_SIZE - len)
        return -EFAULT;

    if(copy_from_user(buff + len, user_buff, count))
        return -EFAULT;

    printk(KERN_DEBUG "Buff after = %s\n\n", buff);

    return count;
}
 
static ssize_t read(struct file *file, char __user *user_buff, size_t count, loff_t *offset) {

    int len = strlen(buff) + 1;
    if(*offset > 0 || count < len)
        return 0;
    
    if(copy_to_user(user_buff, buff, len))
        return -EFAULT;

    printk(KERN_DEBUG "len = %d\n", len);
    *offset = len;
    return len;
}
 
static struct file_operations operations = {

    .owner = THIS_MODULE,
    .read = read,
    .write = write,

};

static int __init test_task_init(void) {

    proc_entry = proc_create("test_task",0666,NULL,&operations);

    return 0;
}
static void __exit test_task_exit(void) {

    proc_remove(proc_entry);
}

module_init(test_task_init);
module_exit(test_task_exit);