#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mubeg");
MODULE_DESCRIPTION("A loadable Linux module as AVSoft Test task.");
MODULE_VERSION("0.1");

#define MAX_BUFF_SIZE 10

static char buff[MAX_BUFF_SIZE] = {};

static struct proc_dir_entry *proc_entry = NULL;

static ssize_t write(struct file *file, const char __user *user_buff, size_t count, loff_t *offset) {

    int len = strlen(buff);     // len = number of characters

    if(count > MAX_BUFF_SIZE - len - 1) // intended input > remaining space (MAX - len - '\0')
        return -EFAULT;

    if(copy_from_user(buff + len, user_buff, count))
        return -EFAULT;

    buff[len + count] = '\0';   // add '\0' after appending a string

    return count;
}
 
static ssize_t read(struct file *file, char __user *user_buff, size_t count, loff_t *offset) {


    int len = strlen(buff) + 1;     // len = number of characters + '\0'
    if(*offset > 0 || count < len)  // no_offset or output buffer < intended output
        return 0;
    
    if(copy_to_user(user_buff, buff, len))
        return -EFAULT;

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