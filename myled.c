#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/io.h>
#include <linux/timer.h>
#include <linux/uaccess.h>
#include <linux/jiffies.h>

MODULE_AUTHOR("Keita Kawakami");
MODULE_DESCRIPTION("driver for LED control");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.0.1");

#define LED_GPIO1 25
#define LED_GPIO2 24 

static dev_t dev;
static struct cdev cdv;
static struct class *cls = NULL;
static volatile u32 *gpio_base = NULL;


static ssize_t led_write(struct file* filp, const char* buf, size_t count, loff_t* pos){
    char c;
    if(copy_from_user(&c,buf,sizeof(char))){
        return -EFAULT;
    }
    if(c == '0'){
        gpio_base[10] = 1 << LED_GPIO1;
		gpio_base[10] = 1 << LED_GPIO2;
    }
    else if(c == '1'){
        gpio_base[7] = 1 << LED_GPIO1;
    }
	else if(c == '2'){
		gpio_base[7] = 1 << LED_GPIO2;
	}
	else if(c == '3'){
		gpio_base[7] = 1 << LED_GPIO1;
		gpio_base[7] = 1 << LED_GPIO2;
	}
    printk(KERN_INFO "led_write is called\n");
    return 1;
}

static struct file_operations led_fops = {
        .owner = THIS_MODULE,
        .write = led_write
};

static int __init init_mod(void)
{
    int retval;
    gpio_base = ioremap_nocache(0x3f200000, 0xA0);
    
    const u32 pin1 = LED_GPIO1;
    const u32 pin2 = LED_GPIO2;

    const u32 index = pin1/10;//GPFSEL2
    const u32 shift1 = (pin1%10)*3;//15bit
	const u32 shift2 = (pin2%10)*3;
    const u32 mask1 = ~(0x7 << shift1);//11111111111111000111111111111111
	const u32 mask2 = ~(0x7 << shift2);
    gpio_base[index] = (gpio_base[index] & mask1 & mask2) | (0x1 << shift1) | (0x1 << shift2);


    retval =  alloc_chrdev_region(&dev, 0, 1, "myled");
    if(retval < 0){ 
        printk(KERN_ERR "alloc_chrdev_region failed.\n");
        return retval;
    }   
    cdev_init(&cdv, &led_fops);
    retval = cdev_add(&cdv, dev, 1); 
    if(retval < 0){ 
        printk(KERN_ERR "cdev_add failed. major:%d, minor:%d",MAJOR(dev),MINOR(dev));
        return retval;
    }   
    cls = class_create(THIS_MODULE,"myled");
    if(IS_ERR(cls)){
        printk(KERN_ERR "class_create failed.");
        return PTR_ERR(cls);
    }   
    device_create(cls, NULL, dev, NULL, "myled%d",MINOR(dev));
    return 0;
}

static void __exit cleanup_mod(void)
{
    cdev_del(&cdv);
    device_destroy(cls,dev);
    class_destroy(cls);
    unregister_chrdev_region(dev, 1); 
    printk(KERN_INFO "%s is unloaded. major:%d\n",__FILE__,MAJOR(dev));
}

module_init(init_mod);
module_exit(cleanup_mod);
