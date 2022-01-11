/***************************************************************************//**
*  \file       test_tools.c
*
*  \details    Simple Linux device driver (File Operations)
*
*  \author     EmbeTronicX
*
* *******************************************************************************/
#include <linux/kernel.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
 
dev_t dev = 0;
static struct class *dev_class;
static struct cdev test_cdev;
 
struct task_info {
	char alg[50];
	int res;
};
 
/*
** Function Prototypes
*/
static int      __init test_tools_init(void);
static void     __exit test_tools_exit(void);
static int      test_open(struct inode *inode, struct file *file);
static int      test_release(struct inode *inode, struct file *file);
static ssize_t  test_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t  test_write(struct file *filp, const char *buf, size_t len, loff_t * off);
static ssize_t  test_unlocked_ioctl(struct file *filp, unsigned int cmd, struct task_info* arg);
static int do_A(struct task_info*);
static int do_B(struct task_info*);
 
typedef int (*alg_do) (struct task_info*);
 
struct alg_handle {
	char alg_name[50];
	alg_do alg_func;
};
 
struct alg_handle alg_handle_tables[] = {
	{"AAA", do_A},
	{"BBB", do_B},
};
 
static struct file_operations fops =
{
    .owner      = THIS_MODULE,
    .read       = test_read,
    .write      = test_write,
    .open       = test_open,
    .release    = test_release,
	.unlocked_ioctl = test_unlocked_ioctl,
};
/*
** This fuction will be called when we open the Device file
*/
static int test_open(struct inode *inode, struct file *file)
{
        printk(KERN_INFO "Driver Open Function Called...!!!\n");
        return 0;
}
/*
** This fuction will be called when we close the Device file
*/
static int test_release(struct inode *inode, struct file *file)
{
        printk(KERN_INFO "Driver Release Function Called...!!!\n");
        return 0;
}
/*
** This fuction will be called when we read the Device file
*/
static ssize_t test_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
	char *a = NULL;
	int i = 0;
	
	printk(KERN_INFO "Driver Read Function Called...!!!\n");
	a = kmalloc(10,GFP_KERNEL);
	for(;i<9;i++) {
		a[i] = i+'a';
	}
	copy_to_user(buf,a,9);
	return 0;
}
/*
** This fuction will be called when we write the Device file
*/
static ssize_t test_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
	char *a=NULL;
	int i = 0;
	
	printk(KERN_INFO "Driver Write Function Called...!!!\n");
	
	a = kmalloc(len,GFP_KERNEL);
	copy_from_user(a,buf,len);
 
	for(;i<9;i++) {
		printk("%d ", a[i]);
	}
	printk("\n");
	return len;
}
/*
** This fuction will be called when we ioctl the Device file
*/
static ssize_t test_unlocked_ioctl(struct file *filp, unsigned int cmd, struct task_info* arg)
{
	struct task_info *tf;
	int num_cmds = 0;
	int index;
	alg_do fn;
	int ret = 0;
	
	tf = kzalloc(sizeof(struct task_info), GFP_KERNEL);
	if (!tf) {
		printk("tf alloc failed.\n");
		return -ENOMEM;
	}
 
	if (copy_from_user(tf, arg, sizeof(*tf))) {
		printk("copy_from_user failed.\n");
		return -EFAULT;
	}
	num_cmds = ARRAY_SIZE(alg_handle_tables);
	for (index = 0; index < num_cmds; index ++) {
		if (!strcmp(tf->alg, alg_handle_tables[index].alg_name)) {
			fn = alg_handle_tables[index].alg_func;
			ret = fn(tf);
			printk("ret:%d res:%d\n", ret, tf->res);
			break;
		}
	}
 
	ret = copy_to_user(arg, tf, sizeof(struct task_info));
	printk("copy_to_user ret:%d.\n", ret);
 
	return ret;
}
 
static int do_A(struct task_info *tf) {
	tf->res = 10;
	
	printk("do somethin in algA\n");
 
	return 0;
}
static int do_B(struct task_info *tf) {
	tf->res = 0;
	
	printk("do somethin in algB\n");
 
	return 0;
}
 
/*
** Module Init function
*/
static int __init test_tools_init(void)
{
        /*Allocating Major number*/
        if((alloc_chrdev_region(&dev, 0, 1, "test_tools")) <0){
                printk(KERN_INFO "Cannot allocate major number\n");
                return -1;
        }
        printk(KERN_INFO "Major = %d Minor = %d \n",MAJOR(dev), MINOR(dev));
        /*Creating cdev structure*/
        cdev_init(&test_cdev,&fops);
        /*Adding character device to the system*/
        if((cdev_add(&test_cdev,dev,1)) < 0){
            printk(KERN_INFO "Cannot add the device to the system\n");
            goto r_class;
        }
        /*Creating struct class*/
        if((dev_class = class_create(THIS_MODULE,"test_class")) == NULL){
            printk(KERN_INFO "Cannot create the struct class\n");
            goto r_class;
        }
        /*Creating device*/
        if((device_create(dev_class,NULL,dev,NULL,"test_tools")) == NULL){
            printk(KERN_INFO "Cannot create the Device 1\n");
            goto r_device;
        }
        printk(KERN_INFO "Device Driver Insert...Done!!!\n");
        return 0;
r_device:
        class_destroy(dev_class);
r_class:
        unregister_chrdev_region(dev,1);
        return -1;
}
/*
** Module exit function
*/
static void __exit test_tools_exit(void)
{
        device_destroy(dev_class,dev);
        class_destroy(dev_class);
        cdev_del(&test_cdev);
        unregister_chrdev_region(dev, 1);
        printk(KERN_INFO "Device Driver Remove...Done!!!\n");
}
module_init(test_tools_init);
module_exit(test_tools_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("lanzhihui@huawei.com");
MODULE_DESCRIPTION("test_tools");
MODULE_VERSION("1.0");
