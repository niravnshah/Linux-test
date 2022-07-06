#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init hello_start(void)
{
    printk(KERN_INFO "Loading hello module....\n");
    printk(KERN_INFO "Hello World from first kenrel module.. \n");
    return 0;
}

static void __exit hello_end(void)
{
    printk(KERN_INFO "Unloading hello module... bye bye....\n");
}

module_init(hello_start);
module_exit(hello_end);

MODULE_AUTHOR("NNS");
MODULE_DESCRIPTION("Hello World Example");
MODULE_LICENSE("GPL");
