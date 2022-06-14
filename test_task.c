/*! \file test_task.c
    \brief Source code file
    
    This project is a testing task for AVSoft company.
    Level: Junior
    Direction: Linux kernel modules
    */

//#define DEBUG

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/version.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mubeg");
MODULE_DESCRIPTION("A loadable Linux module as AVSoft Test task.");
MODULE_VERSION("1.0");

/**
 * Unified buffer size variable [unused].
 */
#define MAX_BUFF_SIZE 4000

/**
 * Type of a stored element.
 */
typedef char Elem_t;

/**
 * Size of an array of data in one list element.
 */
#define ELEM_DATA_SIZE 4000

/**
 * List structure for use with linux kernel list.h.
 */
typedef struct elem_list {

    /**
     * Array of elements in list.
     * 
     * Here it serves a purpose of localizing data into chuncks of ~4KB
     */ 
    Elem_t data[ELEM_DATA_SIZE];
    /**
     * Size of valid data stored in data
     */
    size_t size;
    struct list_head list;

} elem_list_t;

/**
 * Creates list with lst_for_data as head
 */
LIST_HEAD(list_for_data);

/*!
 Adds an initialized empty element into the end of list.
 Initialises size = 0 JIC.
 \param lst pointer to a list head
 \return Error code
 */
static int list_add_elem(struct list_head *lst){

    pr_devel("Function %s enter\n", __PRETTY_FUNCTION__);

    elem_list_t *elem = kcalloc(sizeof(Elem_t), sizeof(elem_list_t), GFP_KERNEL);

    if (!elem)
        return -ENOMEM;

    elem->size = 0;
    list_add_tail(&elem->list, lst);

    pr_devel("Function %s exit\n", __PRETTY_FUNCTION__);

    return 0;
}

/**
 * Destuctor for all of the list
 * \param lst pointer to a list head
 */
static void list_destroy(struct list_head *lst){

    pr_devel("Function %s enter\n", __PRETTY_FUNCTION__);

    struct list_head *i = NULL, *n = NULL;
    elem_list_t *elem = NULL;

    list_for_each_safe(i, n, lst) {
        elem = list_entry(i, elem_list_t, list);
        list_del(i);
        kfree(elem);
    }

    pr_devel("Function %s exit\n", __PRETTY_FUNCTION__);

}

/**
 * Writes to list.
 * Appends logical string of data with new input. Performs search {potenrial for optimization} for non-full member of list and add to it's data untill it runs out of space, then add new node to list.
 * @param file pointer to a file structure
 * @param user_buff pointer to a buffer in user space from which the copying will be performed
 * @param[in] count number of bytes to write
 * @param[out] offset pointer to offset after partial write
 * @return number of written bytes
 */
static ssize_t write(struct file *file, const char __user *user_buff, size_t count, loff_t *offset) {

    pr_devel("Function %s enter\n", __PRETTY_FUNCTION__);

    struct list_head *i = list_for_data.next; // i = first actual page
    elem_list_t *elem = NULL;
    size_t remaining = count;

    while(remaining > 0){

        pr_devel("Function %s enter while loop, remaining = %lu\n", __PRETTY_FUNCTION__, remaining);

        elem = list_entry(i, elem_list_t, list);

        pr_devel("Function %s in while loop brk#1\n", __PRETTY_FUNCTION__);

        if(list_is_last(&elem->list, &list_for_data) && elem->size == ELEM_DATA_SIZE){

            pr_devel("Function %s in while loop brk#2\n", __PRETTY_FUNCTION__);
            list_add_elem(&list_for_data);
            i = i->next;
            continue;
        }
        else if(elem->size == ELEM_DATA_SIZE){
        pr_devel("Function %s in while loop brk#3\n", __PRETTY_FUNCTION__);
            i = i->next;
            continue;
        }

        pr_devel("Function %s in while loop brk#4\n", __PRETTY_FUNCTION__);

        size_t len = min(ELEM_DATA_SIZE - elem->size, remaining);
        
        if(copy_from_user(elem->data + elem->size, user_buff, len))
            return -EFAULT;

        *offset += len;
        user_buff += len;
        remaining -= len;
        elem->size += len;

    }

    pr_devel("Function %s exit\n", __PRETTY_FUNCTION__);

    return count;
}

/**
 * Reads from list.
 * Reads elements of list accordingly to offset and outputs them to user buffer.
 * @param file pointer to a file structure
 * @param user_buff pointer to a buffer in user space into which the copying will be performed
 * @param[in] count number of bytes to read
 * @param offset pointer to offset
 * @return number of read bytes
 */
static ssize_t read(struct file *file, char __user *user_buff, size_t count, loff_t *offset) {

    pr_devel("Function %s enter, count = %ld, offset = %llu\n", __PRETTY_FUNCTION__, count, *offset);

    struct list_head *i = NULL;
    elem_list_t *elem = NULL;
    size_t sum = 0;
    loff_t logical_offset = *offset;

    list_for_each(i, &list_for_data) {

        pr_devel("Function %s in list_loop\n", __PRETTY_FUNCTION__);

        if(logical_offset >= ELEM_DATA_SIZE){
            logical_offset -= ELEM_DATA_SIZE;
            continue;
        }

        pr_devel("Function %s in list_loop brk#0\n", __PRETTY_FUNCTION__);

        elem = list_entry(i, elem_list_t, list);

        size_t len = elem->size;
        loff_t local_offset = 0;

        if(logical_offset > 0){
            if(logical_offset < len){
                len -= logical_offset;
                local_offset = logical_offset;
                logical_offset = 0;
            }
            else{

                pr_devel("Function %s exit on big offset\n", __PRETTY_FUNCTION__);

                return 0; // offset too far -> read complete
            }

        }

        len = min(count - sum, len);

        if(copy_to_user(user_buff, elem->data + local_offset, len)){
            pr_devel("Function %s in list_loop brk#3, len = %lu", __PRETTY_FUNCTION__, len);
            return -EFAULT;
        }

        pr_devel("Function %s in list_loop brk#1, sum = %lu, len = %lu\n", __PRETTY_FUNCTION__, sum, len);

        *offset += len;
        user_buff += len;
        sum += len;
    }

    pr_devel("Function %s exit\n", __PRETTY_FUNCTION__);
    
    return sum;
}

/**
 * Linux kernel 5.6.0 moved to a new structure for sharing file operatons of a module
 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(5,6,0)
/**
 * File operations for Linux kernel < 5.6.0.
 */
static struct file_operations operations = {

    .owner = THIS_MODULE,
    .read = read,
    .write = write,

};

#else
/**
 * File operations for Linux kernel >= 5.6.0.
 */
static struct proc_ops operations = {

    .proc_read = read,
    .proc_write = write,

};
#endif
 

/**
 * Proc_fs structure
 */
static struct proc_dir_entry *proc_entry = NULL;

/**
 * Init linux kernel module on load/
 */
static int __init test_task_init(void) {

    pr_devel("Function %s enter\n", __PRETTY_FUNCTION__);

    proc_entry = proc_create("test_task", 0666, NULL, &operations);
    INIT_LIST_HEAD(&list_for_data);
    list_add_elem(&list_for_data);  // Init first page

    pr_devel("Function %s exit\n", __PRETTY_FUNCTION__);


    return 0;
}

/**
 * Deinit linux kernel module on unload.
 */
static void __exit test_task_exit(void) {

    pr_devel("Function %s enter\n", __PRETTY_FUNCTION__);

    proc_remove(proc_entry);
    list_destroy(&list_for_data);

    pr_devel("Function %s exit\n", __PRETTY_FUNCTION__);

}


module_init(test_task_init);
module_exit(test_task_exit);
