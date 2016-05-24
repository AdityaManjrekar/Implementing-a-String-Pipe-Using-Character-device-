#include <linux/init.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/uaccess.h>
#include <asm/uaccess.h>
#include<linux/fs.h>
#include<linux/semaphore.h>
#include<linux/mutex.h>
#include <linux/miscdevice.h>
#include<linux/slab.h>
#include<asm/bitops.h>
#define BUFFER_SIZE 5
MODULE_LICENSE("DUAL BSD/GPL");
/*
	Function Declarations
*/
static int my_open(struct inode *, struct file *);  
static ssize_t my_read(struct file *file, char __user * out, size_t size, loff_t * off);
static ssize_t my_write(struct file *file,const char __user * out, size_t size, loff_t * off);
static int my_release(struct inode *inod,struct file *fil);
/*
	Global Varaibles
*/
char *buffer[BUFFER_SIZE];// = {"Hello","World","Hi","Good","Bye"};
int consumerPositionCounter = 0;
int producerPositionCounter = 0;
int numberOfTimesDeviceWasOpened = 0;
/*
	Declaration of Semaphore Variables
*/
static DEFINE_SEMAPHORE(full);
static DEFINE_SEMAPHORE(empty);
static DEFINE_MUTEX(mutex);

static struct file_operations my_fops = {
	.read = my_read,
	.write = my_write,
	.open = my_open,
	.release = my_release
};

static struct miscdevice my_misc_device = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "mypipe",
	.fops = &my_fops 
};


// called when module is installed
int __init init_module()
{
	int registeredValue;
	registeredValue = misc_register(&my_misc_device);
	if(registeredValue<0)
			printk(KERN_ALERT "Pipe Device Not Registered\n");		
	else
		{
			/*
				Initialising Semaphore values
			*/
			printk(KERN_ALERT "Pipe Device Registered\n");		
			sema_init(&full,0);
			sema_init(&empty,BUFFER_SIZE);
			mutex_init(&mutex);
		}
	return registeredValue;
}

// called when module is removed
void __exit cleanup_module()
{
	misc_deregister(&my_misc_device);
	printk(KERN_ALERT "Pipe Device Closed\n");
}
static int my_open(struct inode *inod,struct file *fil)
{
	numberOfTimesDeviceWasOpened++; 
	printk(KERN_ALERT "Device opened %d times\n ",numberOfTimesDeviceWasOpened);		
	return 0;
}
static int my_release(struct inode *inod,struct file *fil)
{
	numberOfTimesDeviceWasOpened--; 
	printk(KERN_ALERT "Device Released %d times\n",numberOfTimesDeviceWasOpened);
	return 0;
}
static ssize_t my_read(struct file *file, char __user * out, size_t size, loff_t * off)
{
	int errorFlag=0;
	char buf[100];
	errorFlag = down_interruptible(&full);
	if(errorFlag==0)
	{
		printk(KERN_ALERT "down Full  operation succesfully completed\n");
	}
	else if(errorFlag==-4)
	{
		printk(KERN_ALERT "down Full operation terminated by Ctrl-C\n");
		return -4;
	}
	else
	{
		printk(KERN_ALERT "down Full operation unsuccessful\n");
		return -1;
	}
	errorFlag =	mutex_lock_interruptible(&mutex);
	printk(KERN_ALERT "errorFlag value in read mutex_lock_interruptible: %d\n",errorFlag);
	if(errorFlag==0)
	{
		printk(KERN_ALERT "Mutex lock interruptible succesfully completed\n");
	}
	else if(errorFlag==-4)
	{
		printk(KERN_ALERT "Mutex lock interruptible operation terminated by Ctrl-C\n");
		return -4;
	}
	else
	{
		printk(KERN_ALERT "Mutex lock interruptible unsuccessful\n");
		return -1;
	}
	/*
		Entering Critical Section
	*/
	if(consumerPositionCounter%BUFFER_SIZE==0)
		consumerPositionCounter =0;
	sprintf(buf, buffer[consumerPositionCounter]);
	errorFlag = copy_to_user(out, buf, strlen(buf)+1);
	printk(KERN_ALERT "errorFlag value in read copy_to_user: %d\n",errorFlag);
	if(errorFlag!=0)
	{
			printk(KERN_ALERT "Copied to user variable unsuccessful\n");		
			return -1;
	}
	else
	{
			printk(KERN_ALERT "Copied to user variable successful Value %s\n",out);
	}
	/*
		Exiting Critical Section
	*/
	mutex_unlock(&mutex);
	printk(KERN_ALERT "Mutex Unlock interruptible succesfully completed\n");
	up(&empty);
	printk(KERN_ALERT "up Empty operation completed\n");
	consumerPositionCounter++;
	return 0;
}
static ssize_t my_write(struct file *file,const char __user * out, size_t size, loff_t * off)
{
	int errorFlag=0;
	errorFlag = down_interruptible(&empty);
	if(errorFlag==0)
	{
		printk(KERN_ALERT "down Empty operation succesfully completed\n");
	}
	else if(errorFlag==-4)
	{
		printk(KERN_ALERT "down Empty operation terminated by Ctrl-C\n");
		return -4;
	}
	else
	{
		printk(KERN_ALERT "down Empty operation unsuccessful\n");
		return -1;
	}
	
	errorFlag = mutex_lock_interruptible(&mutex);
	if(errorFlag==0)
	{
		printk(KERN_ALERT "Mutex lock interruptible succesfully completed\n");
	}
	else if(errorFlag==-4)
	{
		printk(KERN_ALERT "Mutex lock interruptible operation terminated by Ctrl-C\n");
		return -4;
	}
	else
	{
		printk(KERN_ALERT "Mutex lock interruptible unsuccessful\n");
		return -1;
	}
	if(producerPositionCounter%BUFFER_SIZE==0)
		producerPositionCounter =0;
	buffer[producerPositionCounter] = kmalloc(sizeof(out), GFP_KERNEL);
	errorFlag = copy_from_user(buffer[producerPositionCounter], out, strlen(out)+1);
	if(errorFlag!=0)
	{
			printk(KERN_ALERT "Copied from user variable unsuccessful\n");		
			return -1;
	}
	else
	{
			printk(KERN_ALERT "Copied from user variable successful string is %s\n",buffer[producerPositionCounter]);
	}
	mutex_unlock(&mutex);
	printk(KERN_ALERT "Mutex Unlock interruptible succesfully completed\n");
	up(&full);
	printk(KERN_ALERT "up Full operation completed\n");
	producerPositionCounter++;
	return 0;
}
