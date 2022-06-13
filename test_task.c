#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/list.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mubeg");
MODULE_DESCRIPTION("A loadable Linux module as AVSoft Test task.");
MODULE_VERSION("0.12");

#define __LOCATION__  __FILE__, __LINE__, __PRETTY_FUNCTION__

#define MAX_BUFF_SIZE 4000

typedef char Elem_t;
#define ELEM_DATA_SIZE 4001 // 1 is reserved for '\0'


typedef struct elem_list {

    Elem_t data[ELEM_DATA_SIZE];
    size_t size;
    struct list_head list;

} elem_list_t;

LIST_HEAD(list_for_data);

static int list_add_elem(struct list_head *lst)
{

    printk(KERN_DEBUG "Function %s enter\n", __PRETTY_FUNCTION__);

    elem_list_t *elem = kmalloc(sizeof(elem_list_t), GFP_KERNEL);

    if (!elem)
        return -ENOMEM;

    elem->data[0] = 0;
    elem->size = 0;
    list_add(&elem->list, lst);

    printk(KERN_DEBUG "Function %s exit\n", __PRETTY_FUNCTION__);

    return 0;
}

static void list_destroy(struct list_head *lst)
{

    printk(KERN_DEBUG "Function %s enter\n", __PRETTY_FUNCTION__);

    struct list_head *i = NULL, *n = NULL;
    elem_list_t *elem = NULL;

    list_for_each_safe(i, n, lst) {
        elem = list_entry(i, elem_list_t, list);
        list_del(i);
        kfree(elem);
    }

    printk(KERN_DEBUG "Function %s exit\n", __PRETTY_FUNCTION__);

}

static struct proc_dir_entry *proc_entry = NULL;

static ssize_t write(struct file *file, const char __user *user_buff, size_t count, loff_t *offset) {

    printk(KERN_DEBUG "Function %s enter\n", __PRETTY_FUNCTION__);

    struct list_head *i = &list_for_data;
    elem_list_t *elem = NULL;
    size_t remaining = count;

    while(remaining > 0){

        printk(KERN_DEBUG "Function %s enter while loop, remaining = %d\n", __PRETTY_FUNCTION__, remaining);

        elem = list_entry(i, elem_list_t, list);

        if(list_is_last(&elem->list, &list_for_data) && elem->size == ELEM_DATA_SIZE){
            list_add_elem(&list_for_data);
            i = i->next;
            continue;
        }
        else if(elem->size == ELEM_DATA_SIZE - 1){
            i = i->next;
            continue;
        }

        size_t len = min(ELEM_DATA_SIZE - 1 - elem->size, remaining);       // len = number of characters + '\0'
        
        if(copy_from_user(elem->data + elem->size, user_buff, len))
            return -EFAULT;

        *offset += len;
        user_buff += len;
        remaining -= len;
        elem->size += len;

        *(elem->data + elem->size) = '\0';   // add '\0' after appending a string

    }

    printk(KERN_DEBUG "Function %s exit\n", __PRETTY_FUNCTION__);

    return count;
}
 
static ssize_t read(struct file *file, char __user *user_buff, size_t count, loff_t *offset) {

    printk(KERN_DEBUG "Function %s enter\n", __PRETTY_FUNCTION__);

    struct list_head *i = NULL;
    elem_list_t *elem = NULL;
    size_t sum = 0;

    if(*offset > 0) // no_offset
        return 0;

    list_for_each(i, &list_for_data) {

        elem = list_entry(i, elem_list_t, list);

        size_t len = elem->size == ELEM_DATA_SIZE - 1 ? elem->size : elem->size + 1;     // len = number of characters + '\0'
        if(count < len)  // output buffer < intended output
            return sum;

        if(copy_to_user(user_buff, elem->data, len))
            return -EFAULT;

        *offset += len;
        user_buff += len;
        sum += len;
    }

    printk(KERN_DEBUG "Function %s exit\n", __PRETTY_FUNCTION__);
    
    return sum;
}
 
static struct file_operations operations = {

    .owner = THIS_MODULE,
    .read = read,
    .write = write,

};

static int __init test_task_init(void) {

    printk(KERN_DEBUG "Function %s enter\n", __PRETTY_FUNCTION__);

    proc_entry = proc_create("test_task", 0666, NULL, &operations);
    INIT_LIST_HEAD(&list_for_data);

    printk(KERN_DEBUG "Function %s exit\n", __PRETTY_FUNCTION__);


    return 0;
}
static void __exit test_task_exit(void) {

    printk(KERN_DEBUG "Function %s enter\n", __PRETTY_FUNCTION__);

    proc_remove(proc_entry);
    list_destroy(&list_for_data);

    printk(KERN_DEBUG "Function %s exit\n", __PRETTY_FUNCTION__);

}

module_init(test_task_init);
module_exit(test_task_exit);