#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int helloworld_init(void)
{
    printk(KERN_INFO "Hello World \n");
    return 0;
}

static void  helloworld_exit(void)
{
    printk(KERN_INFO "Goodbye World \n");
}

module_init(helloworld_init);
module_exit(helloworld_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Chengzhi Tan");
MODULE_DESCRIPTION("hello world module.");
