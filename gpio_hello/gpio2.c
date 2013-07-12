/*///////////////////////////////////////////////////////////////////////////////////////
//Program Name: gpio_device_driver	Author: Haichao Zhang
//
//Plat_form: Beaglebone Black(Cross Compile Ubuntu version)
//
//latest modified date:June, 23, 2013
//
//Functionality Description: This is for teaching purpose to show how to write a simple
// device driver. Basically, you need to rewrite serveral functions to finish a driver. 
// I leave out ioctl() in this program so that the code won't cost your too much time.
// But if you want to learn more about character device driver. You must learn how to 
// use this function.
//
//Please read the code carefully and goto the source code browser before asking me any 
// question. GOOD LUCK!
*//////////////////////////////////////////////////////////////////////////////////////

//They are what you won't forget when you want to write a module
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

#include <linux/platform_device.h>

//gpio.h is important in this program. Please refer the code in kernel source.
#include <linux/gpio.h>

#include <linux/fs.h>
#include <linux/errno.h>
#include <asm/uaccess.h>
#include <linux/version.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/timer.h>

#define GPIO_NUMBER    47     //macro for p8 no.15 pin whose pin number is GPIO1_15=32*1+15=47.
//---modified---
#define GPIO_EXTRA	46

struct timer_list my_timer;


static dev_t first;         // Global variable for the first device number(major+minor)
static struct cdev char_dev;     // Global variable for the character device structure
static struct class *cl;     // Global variable for the device class
static char pin[10]="my_pins";
static int init_result;


//---modified
void led_blink_off(unsigned long data);
void led_blink_off_fast(unsigned long data);

void led_blink_on(unsigned long data)
{
	//---set led on
	gpio_set_value(GPIO_NUMBER, 1);
	setup_timer(&my_timer, led_blink_off, 0);
	mod_timer(&my_timer, jiffies+msecs_to_jiffies(500));
}

void led_blink_off(unsigned long data)
{
	//---set led off
	gpio_set_value(GPIO_NUMBER, 0);
	setup_timer(&my_timer, led_blink_on, 0);
	mod_timer(&my_timer, jiffies+msecs_to_jiffies(500));
}


void led_blink_on_fast(unsigned long data)
{
	//---set led on
	gpio_set_value(GPIO_NUMBER, 1);
	setup_timer(&my_timer, led_blink_off_fast, 0);
	mod_timer(&my_timer, jiffies+msecs_to_jiffies(200));
}

void led_blink_off_fast(unsigned long data)
{
	//---set led off
	gpio_set_value(GPIO_NUMBER, 0);
	setup_timer(&my_timer, led_blink_on_fast, 0);
	mod_timer(&my_timer, jiffies+msecs_to_jiffies(200));
}

static ssize_t gpio_read( struct file* F, char *buf, size_t count, loff_t *f_pos )
{
	char buffer[10];
	//gpio_get_value() can be found from the gpio.h, used for reading the value of the gpio pin
	int temp = gpio_get_value(GPIO_NUMBER);

	sprintf( buffer, "%1d" , temp );

	count = sizeof( buffer );

	//copy_to_user is  copy_to_user(A,B,C)
	//A is the file descriptor mostly the file in /dev
	//B is the buffer
	//C is the length of the data
	if( copy_to_user( buf, buffer, count ) )
	{
		return -EFAULT;
	}

	if( *f_pos == 0 )
	{
		*f_pos += 1;
		return 1;
	}
	else
	{
		return 0;
	}

}

static ssize_t gpio_write( struct file* F, const char *buf, size_t count, loff_t *f_pos )
{

	printk(KERN_INFO "Executing WRITE.\n");
	// ----better to add a copy from user function, due to the security issue

	switch( buf[0] )
	{
		case '3':
			del_timer(&my_timer);
			setup_timer(&my_timer, led_blink_on_fast, 0);
			mod_timer(&my_timer, jiffies+msecs_to_jiffies(200));
			break;

		case '2':
			setup_timer(&my_timer, led_blink_on, 0);
			mod_timer(&my_timer, jiffies+msecs_to_jiffies(500));
			break;

		case '1':
			del_timer(&my_timer);
			gpio_set_value(GPIO_NUMBER, 1);
			gpio_set_value(GPIO_EXTRA, 1);
		//	printk(KERN_INFO "Try to Blink.\n");
		//	setup_timer(&my_timer, led_blink, 0);
		//	mod_timer(&my_timer, jiffies + msecs_to_jiffies(200));
			break;

		case '0':
			del_timer(&my_timer);
			gpio_set_value(GPIO_NUMBER, 0);
			gpio_set_value(GPIO_EXTRA, 0);
			break;

		default:
			printk("Wrong option.\n");
			break;
	}

	return count;
}



//we don't do anything special when open
static int gpio_open( struct inode *inode, struct file *file )
{
	return 0;
}


//neither do we have anything to do when close.
static int gpio_close( struct inode *inode, struct file *file )
{
	return 0;
}

static struct file_operations FileOps =
{
	.owner        = THIS_MODULE,//This is for the owner identification. Almost all are THIS_MODULE.
	.open         = gpio_open,//open the device(In linux, everything is a file. That is why we say open.)
	.read         = gpio_read,//read from the device
	.write        = gpio_write,//write into the device
	.release      = gpio_close,//close the device
};

static int init_gpio(void)
{
	//alternative way to register the device number. here is 0.
	//init_result = register_chrdev( 0, "gpio", &FileOps );



	/*
	//this part of code can not be permitted so that I change my way.
	if( 0 > gpio_request_one(GPIO_NUMBER,GPIOF_DIR_OUT,pin) )
	{
		
		printk( KERN_ALERT "Device request failed\n" );
		return -1;
	}
	*/



	//set the gpio direction to output and the value to 1.
	if(0<gpio_direction_output(GPIO_NUMBER, 1))
		return -1;
	if(0<gpio_direction_output(GPIO_EXTRA, 1))
		return -1;



	//dynamically allocate the device number for the device to register.
	// &first will store the device number;
	// 0 here is where the minor number start;
	// 1 is how many devices here.
	// this is the file name(device name in /dev) 
	init_result = alloc_chrdev_region( &first, 0, 1, "gpio_drv" );



	
	if( 0 > init_result )
	{
		printk( KERN_ALERT "Device Registration failed\n" );
		return -1;
	}





	//The next a few lines of code are trying to create the file in /dev
	// you need create the class -> device -> /dev/file
	if ( (cl = class_create( THIS_MODULE, "char_device" ) ) == NULL )
	{
		printk( KERN_ALERT "Class creation failed\n" );
		unregister_chrdev_region( first, 1 );
		return -1;
	}

	if( device_create( cl, NULL, first, NULL, "gpio_driver" ) == NULL )
	{
		printk( KERN_ALERT "Device creation failed\n" );
		class_destroy(cl);
		unregister_chrdev_region( first, 1 );
		return -1;
	}

	cdev_init( &char_dev, &FileOps );


	if( cdev_add( &char_dev, first, 1 ) == -1)
	{
		printk( KERN_ALERT "Device addition failed\n" );
		device_destroy( cl, first );
		class_destroy( cl );
		unregister_chrdev_region( first, 1 );
		return -1;
	}

	return 0;

}

void cleanup_gpio(void)
{
//unregister_chrdev( init_result, "gpio" );

	cdev_del( &char_dev );
	device_destroy( cl, first );
	class_destroy( cl );
	unregister_chrdev_region( first, 1 );
	
	//---delete timer
//	del_timer(&my_timer);


	printk(KERN_ALERT "Device unregistered\n");

}

module_init(init_gpio);
module_exit(cleanup_gpio);

MODULE_AUTHOR("Haichao Zhang");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Beaglebone Black gpio driver");
