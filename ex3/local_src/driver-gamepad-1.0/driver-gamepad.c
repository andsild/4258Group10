#include "efm32gg.h"
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <asm/signal.h>
#include <asm/siginfo.h>

/*
	Definitions
*/
#define DRIVER_NAME "driver-gamepad"
#define IRQ_NUMBER_EVEN 17
#define IRQ_NUMBER_ODD 18
#define DATA_LENGTH 0x04

/*
	structs and statics
*/
struct cdev my_cdev = {
    .owner = THIS_MODULE
};

struct class *cl;
struct fasync_struct *async_queue;
static dev_t my_dev_t = 0;
struct resource *gpioPc_memRegion;

/*
   Interrupt handler function
*/
static irq_handler_t interruptHandler(int irq, void *dev_id,
                                      struct pt_regs *regs)
{
    /* Casts */
    uint32_t gpioPc_value = 0;
    void *gpioPc_base;

    /* Create virtual memory */
    gpioPc_base = ioremap_nocache(gpioPc_memRegion->start, GPIO_PC_LENGTH);

    /* read interrupt flag */
    gpioPc_value = ioread32(gpioPc_base + IF_OFFSET);

    /* Clear interrupt pending bit */
    iowrite32(gpioPc_value, gpioPc_base + IFC_OFFSET);

    /* send a signal to the user program */
    if (async_queue) {
        kill_fasync(&async_queue, SIGIO, POLL_IN);
    }

    /* Release virtual memory */
    iounmap(gpioPc_base);

    return (irq_handler_t) IRQ_HANDLED;
}

/* 
	fasync-function
*/
static int my_fasync(int fd, struct file *filp, int mode)
{
    return fasync_helper(fd, filp, mode, &async_queue);
}

/*
   Open-function called when the device-driver is opened
*/
static int my_open(struct inode *inode, struct file *filp)
{

    /* Initializing interrupt requests */
    if (request_irq
        (IRQ_NUMBER_EVEN, (irq_handler_t) interruptHandler, 0, DRIVER_NAME,
         &my_cdev) != 0) {
        printk("Interrupt initialization error, EVEN");
    }
    if (request_irq
        (IRQ_NUMBER_ODD, (irq_handler_t) interruptHandler, 0, DRIVER_NAME,
         &my_cdev) != 0) {
        printk("Interrupt initialization error, ODD");
    }

    return 0;
}

/*
   Release-function called when the device-driver is closed
*/
static int my_release(struct inode *inode, struct file *filp)
{
    printk("Closing driver...");

    /* Free irq */
    free_irq(IRQ_NUMBER_EVEN, &my_cdev);
    free_irq(IRQ_NUMBER_ODD, &my_cdev);

    printk("Driver closed!");
    return 0;
}

/*
   Function for reading data from the device, used by the user program
*/
static ssize_t my_read(struct file *filp, char __user *buff, size_t count,
                       loff_t *offp)
{
    /* Casts */
    void *gpioPc_base;
    uint32_t data;

    /* Create virtual memory */
    gpioPc_base = ioremap_nocache(gpioPc_memRegion->start, GPIO_PC_LENGTH);

    /* Read data from pin-register */
    
    printk("read from region %d\n", gpioPc_base);
    data = ioread32(gpioPc_base + DIN_OFFSET);
    copy_to_user(buff, &data, DATA_LENGTH);
    return 1;
};

/*
   Function for writing data to the device, not used.
*/
static ssize_t my_write(struct file *filp, const char __user * buff,
                        size_t count, loff_t * offp)
{
    return 1;
};

/*
	Struct defining functions for file-operations
*/
static struct file_operations my_fops = {
    .owner = THIS_MODULE,
    .read = my_read,
    .write = my_write,
    .open = my_open,
    .release = my_release,
    .fasync = my_fasync
};

/*
 * template_init - function to insert this module into kernel space
 *
 * This is the first of two exported functions to handle inserting this
 * code into a running kernel
 *
 * Returns 0 if successfull, otherwise -1
 */

static int __init template_init(void)
{
    /* Casts */
    void *gpioPc_base;
    int result;

    printk("Hello World, here is your module speaking\n");

    /* Requesting memory region and setting up memory map */
    printk("Allocating memory region, step 1\n");
    if ((gpioPc_memRegion =
         request_mem_region(GPIO_PC_BASE, GPIO_PC_LENGTH,
                            DRIVER_NAME)) == NULL) {
        printk("GPIO_PC_BASE allocation error");
        return -1;
    }
    gpioPc_base = ioremap_nocache(gpioPc_memRegion->start, GPIO_PC_LENGTH);

    /* Setting up GPIO pins */
    printk("Setting up GPIO-pins, step 2\n");
    iowrite32(0x33333333, gpioPc_base + MODEL_OFFSET);  /* set pins C0-7 as input */
    iowrite32(0xff, gpioPc_base + DOUT_OFFSET); /* enabling internal pullup */

    /* Enable interrupts in the controller */
    iowrite32(0x22222222, gpioPc_base + EXTIPSELL_OFFSET);
    iowrite32(0xff, gpioPc_base + EXTIFALL_OFFSET);
    iowrite32(0x00ff, gpioPc_base + IEN_OFFSET);
    iowrite32(0xff, gpioPc_base + IFC_OFFSET);

    /* Dynamically allocating the device numbers */
    printk("Allocating device numbers, step 3 \n");
    result = alloc_chrdev_region(&my_dev_t, 0, 1, "devno");
    if (result < 0) {
        printk(KERN_ERR "chrdev alloc error");
        unregister_chrdev_region(my_dev_t, 1);
        return -1;
    }

    /* Initializing the char device and adding it to the kernel */
    printk("Making char device, step 4 \n");
    cdev_init(&my_cdev, &my_fops);
    result = cdev_add(&my_cdev, my_dev_t, 1);
    if (result < 0) {
        printk(KERN_ERR "add cdev error");
        return -1;
    }

    /* Creating the dev-file */
    printk("Creating dev file, step 5 \n");
    cl = class_create(THIS_MODULE, DRIVER_NAME);
    device_create(cl, NULL, my_dev_t, NULL, DRIVER_NAME);

    /* Unmapping virtual memory */
    iounmap(gpioPc_base);

    printk("Gamepad locked and loaded, ready for hardcore step mania");

    return 0;
}

/*
 * template_cleanup - function to cleanup this module from kernel space
 *
 * This is the second of two exported functions to handle cleanup this
 * code from a running kernel
 */

static void __exit template_cleanup(void)
{

    /* Releasing memory region */
    release_mem_region(GPIO_PC_BASE, GPIO_PC_LENGTH);

    /* Deleting character device */
    cdev_del(&my_cdev);

    /* Removing device number and class */
    device_destroy(cl, my_dev_t);
    class_destroy(cl);
    unregister_chrdev_region(my_dev_t, 1);

    printk("Wow, that was hardcore.. You're amazing\n");
}

/* Telling the module the name of its init and exit functions */
module_init(template_init);
module_exit(template_cleanup);

MODULE_DESCRIPTION("Device driver for gamepad.");
MODULE_LICENSE("GPL");
