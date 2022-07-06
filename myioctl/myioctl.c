#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include "myioctl.h"

dev_t myioctl_dev = 0;
static struct class *myioctl_class;
static struct cdev myioctl_cdev;

static int __init myioctl_init(void);
static void __exit myioctl_exit(void);

static int myioctl_open(struct inode *inode, struct file *file);
static int myioctl_release(struct inode *inode, struct file *file);

static ssize_t myioctl_read(struct file *file, char __user *buf, size_t len, loff_t *off);
static ssize_t myioctl_write(struct file *file, const char *buf, size_t len, loff_t *off);

static long myioctl_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

static struct file_operations fops = {
    .owner      = THIS_MODULE,
    .open       = myioctl_open,
    .release    = myioctl_release,
    .read       = myioctl_read,
    .write      = myioctl_write,
    .unlocked_ioctl = myioctl_ioctl,
};

static int myioctl_open(struct inode *inode, struct file *file)
{
    pr_info("myioctl --> Device file opened...\n");
    return 0;
}

static int myioctl_release(struct inode *inode, struct file *file)
{
    pr_info("myioctl --> Device file released...\n");
    return 0;
}

static ssize_t myioctl_read(struct file *file, char __user *buf, size_t len, loff_t *off)
{
    pr_info("myioctl --> Read function called...\n");
    return 0;
}

static ssize_t myioctl_write(struct file *file, const char *buf, size_t len, loff_t *off)
{
    pr_info("myioctl -> Write function called...\n");
    return len;
}

static long myioctl_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    static unsigned long value = 0;
    pr_info("myioctl --> ioctl called...\n");

    switch(cmd) {
        case WR_VALUE:
            if(copy_from_user(&value, (int32_t *)arg, sizeof(value))) {
                pr_err("myioctl --> data write error\n");
            }
            pr_info("myioctl --> Value received from user = %ld\n", value);
            break;

        case RD_VALUE:
            pr_info("myioctl --> Value returning to user = %ld\n", value);
            if(copy_to_user((int32_t*)arg, &value, sizeof(value))) {
                pr_err("myioctl --> data read error\n");
            }
            break;

        default:
            pr_info("myioctl --> unsupported\n");
            break;
    }
    return 0;
}

static int __init myioctl_init(void)
{
    pr_info("myioctl_init called\n");

    if((alloc_chrdev_region(&myioctl_dev, 0, 1, "myioctl_dev")) < 0) {
        pr_err("Cannot allocate major number for myioctl_dev\n");
        return -1;
    }

    cdev_init(&myioctl_cdev, &fops);

    if((cdev_add(&myioctl_cdev, myioctl_dev, 1)) < 0) {
        pr_err("Cannot add the device to the system\n");
        goto r_region;
    }

    if((myioctl_class = class_create(THIS_MODULE, "myioctl_class")) == NULL) {
        pr_err("Cannot create the device class\n");
        goto r_cdev;
    }

    if((device_create(myioctl_class, NULL, myioctl_dev, NULL, "myioctl_device")) == NULL) {
        pr_err("Cannot create myioctl device\n");
        goto r_class;
    }

    pr_info("myioctl --> Device creation successful\n");
    return 0;

    device_destroy(myioctl_class, myioctl_dev);
r_class:
    class_destroy(myioctl_class);
r_cdev:
    cdev_del(&myioctl_cdev);
r_region:
    unregister_chrdev_region(myioctl_dev, 1);
    return -1;
}

static void __exit myioctl_exit(void)
{
    pr_info("myioctl_exit called\n");
    device_destroy(myioctl_class, myioctl_dev);
    class_destroy(myioctl_class);
    cdev_del(&myioctl_cdev);
    unregister_chrdev_region(myioctl_dev, 1);
    pr_info("myioctl --> Device removal successful\n");
}

module_init(myioctl_init);
module_exit(myioctl_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nirav Shah");
MODULE_DESCRIPTION("Sample IOCTL implementation");
