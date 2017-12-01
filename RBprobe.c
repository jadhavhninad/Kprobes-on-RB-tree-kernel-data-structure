#include <linux/kernel.h>
#include <linux/kprobes.h>
#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/moduleparam.h>
#include <asm/uaccess.h>
#include <linux/types.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/device.h>
#include <linux/sched.h>
#include <linux/rbtree.h>
#include <linux/time.h>

//#include <x86intrin.h>

#define DEVICE_NAME "RBprobe"  // device name to be created and registered
#define DEVICE_CLASS "rbprobe_cls"     //the device class as shown in /sys/class
#define DEVICES 1
#define LENGTH_LIMIT 256

static dev_t kp_dev_number;
static struct class *kp_dev_data;
static struct device *kp_dev_device;
struct kp_dev;
static struct kprobe kp_kpt;
char output[LENGTH_LIMIT]; //file->private_data
int length=0;
static uint64_t RDTSC(void);

struct kp_dev{
        struct cdev cdev;               /* The cdev structure */
        char name[20];                  /* Name of device*/
}*kp_devp;


struct rb_object {
        struct rb_node my_node;
        int key;
        int data;
}*rb_object_t;


static void dump_state(struct pt_regs *regs)
{
	print_symbol(KERN_INFO "EIP is at %s\n", regs->ip);
	printk(KERN_INFO "eax: %08lx   ebx: %08lx   ecx: %08lx   edx: %08lx\n", regs->ax, regs->bx, regs->cx, regs->dx);
	printk(KERN_INFO "esi: %08lx   edi: %08lx   ebp: %08lx   esp: %08lx\n", regs->si, regs->di, regs->bp, regs->sp);
	printk(KERN_INFO "Process %s (pid: %d, threadinfo=%p task=%p)", current->comm, current->pid, current_thread_info(), current);
}


static int handler_pre(struct kprobe *kp, struct pt_regs *regs){
	printk(KERN_INFO "KPROBE : Inside pre_handler \n");
	printk(KERN_INFO "kp->addr = 0x%p, ip=%lx, flags=0x%lx\n",kp->addr,regs->ip,regs->flags);
	dump_state(regs);
	return 0;
}

static uint64_t RDTSC()
{
  unsigned int hi, lo;
  __asm__ volatile("rdtsc" : "=a" (lo), "=d" (hi));
  return ((uint64_t)hi << 32) | lo;
}

void handler_post(struct kprobe *kp, struct pt_regs *regs, unsigned long flags){
	printk (KERN_INFO "KPROBE : Inside post_handler \n");
	printk(KERN_INFO "post_handler : kp->addr= 0x%p, flags = 0x%lx\n",kp->addr, regs->flags);
	
	int i=0;
	//dumping into buffer	
	length = 0;
	int temp_length=0;
	
	int pid = current->pid;
/*	
	if (pid != globalPid || temp_length >= LENGTH_LIMIT) {
		
		length=temp_length;
		msleep(2000);
		length=0;
		temp_length=0;
		globalPid = pid;
	}
*/	
	
	temp_length += sprintf(output+temp_length,"pid = %d, ",pid);
	size_t addr = kp->addr;
	temp_length += sprintf(output+temp_length,"address = 0x%x, ",addr);
	uint64_t tstamp = RDTSC();
	temp_length += sprintf(output+temp_length,"timestamp (TSC) = %lld, ",tstamp);
	
	//for (i; i<length; i++) printk (KERN_INFO "%c",output[i]);

	struct rb_node *loc =  (struct rb_node *)(void *)regs->r12;
	printk (KERN_INFO "rb_node addr = %x\n",(struct rb_node *)regs->r12);
	struct rb_object *structdata  = container_of(loc, struct rb_object, my_node);
	int keydata = structdata->key;
	temp_length += sprintf(output+temp_length,"rb_object->key = %d\n",keydata);
	length = temp_length;
	
}


static int handler_fault(struct kprobe *kp, struct pt_regs *regs, int trapval){

	printk(KERN_INFO "fault_handler: kp->addr = 0x%p, trap #%d \n",kp->addr,trapval);
	return 0;
}

static int kp_register(int opc_val){
	printk(KERN_INFO "KPROBE : Inside kpregister \n");	
	int ret_val;
	kp_kpt.pre_handler = handler_pre;
        kp_kpt.post_handler = handler_post;
        kp_kpt.fault_handler = handler_fault;
	
	//dummy initialization of kprobe.addr
	if (opc_val == 0) {
		printk(KERN_INFO "KPROBE : OPC_VAL = %d . Probing rb_insert \n",opc_val);
		if (kallsyms_lookup_name("rb_insert") == NULL){
			printk(KERN_INFO "KPROBE : NULL pointer derefference!\n");
			return -1;
		}
		printk(KERN_INFO "KPROBE lookup : %pi \n", (void **)(kallsyms_lookup_name("rb_insert")));
		printk(KERN_INFO "KPROBE lookup offset : %pi \n", (void **)(kallsyms_lookup_name("rb_insert")+0x24));
		kp_kpt.addr = (kprobe_opcode_t *)(void **)kallsyms_lookup_name("rb_insert") + 0x24; // read regs->r12
	} else {
		printk(KERN_INFO "KPROBE : Probing rb_read \n");
		printk(KERN_INFO "KPROBE : lookup : %pi \n", (void **)(kallsyms_lookup_name("rb_read")));
		printk(KERN_INFO "KPROBE lookup offset : %pi \n", (void **)(kallsyms_lookup_name("rb_read")+0x168));
		kp_kpt.addr = (kprobe_opcode_t *)(void **)kallsyms_lookup_name("rb_read") + 0x168; //read regs->r12
	}

        ret_val = register_kprobe(&kp_kpt);
	
	/* Register the kprobe */
  	if (ret_val < 0) {
   	 	printk(KERN_INFO " KPROBE : register_kprobe error, return value=%d\n", ret_val);
    		return -1;
  	}
  	return 0;
	
}


int open(struct inode *inode, struct file *file)
{
 	// Get the per-device structure that contains this cdev */
        kp_devp = container_of(inode->i_cdev, struct kp_dev, cdev);
                
        //Now we can access the driver struct elements.
        file->private_data = kp_devp;
	printk(KERN_INFO "KPROBE : Inside open \n");
	return 0; 
}


int release(struct inode *inode, struct file *file) {
 printk (KERN_INFO "KPROBE : Inside close \n");
 return 0;
}


//Read a node value
ssize_t kp_read_handler(struct file *file, char *buf, size_t count, loff_t *ppos)
{
        struct kp_dev *kp_devp = file->private_data;
        //struct kp_node *seeker = kp_devp->rbt530.kp_node;
        int result=0;
	int i=0;
	printk(KERN_INFO "KPROBE : ");
	for (i; i<length; i++) printk (KERN_INFO "%c",output[i]);         
	printk(KERN_INFO "KPROBE DATA DUMP \n");
       // int  start = length--;
        printk(KERN_INFO "START = %d\n",length);
        int bytes_read = 0;
	while (length == 0) msleep(1000);
        
	while (bytes_read<=length) {
		printk(KERN_INFO "%c,",output[bytes_read]);
		put_user(output[bytes_read], buf++);
                printk(KERN_INFO "%c\n",buf[bytes_read]);
                //count--;
                //start--;
                bytes_read++;
        }
	length = 0;	
        return bytes_read;

}

ssize_t kp_write_handler(struct file *file, const char *buf, size_t count, loff_t *ppos){

	printk(KERN_INFO "KPROBE : Inside write_handler \n");
        int val = buf[0];
        int op_to_do = buf[1];
	printk(KERN_INFO "op_to_do = %d\n",op_to_do);
        if(op_to_do == 1){
		printk(KERN_INFO "KPROBE : Calling kpregister \n");
                return kp_register(val);
       } else {
		printk(KERN_INFO "KPROBE : Calling kpunregister \n");
                unregister_kprobe(&kp_kpt);
                return 0;
        }

}


struct file_operations kp_fops = {
 open:   open,
 write:  kp_write_handler,
 release: release,
 read: kp_read_handler
};


int __init kprobe_init(void)
{
        int ret;

        /* Request dynamic allocation of a device major number */
        if (alloc_chrdev_region(&kp_dev_number, 10, 1, DEVICE_NAME) < 0) {
                        printk(KERN_DEBUG "RB :Can't register device\n"); return -1;
        }

        /* Populate sysfs entries */
        kp_dev_data = class_create(THIS_MODULE, DEVICE_CLASS);

        /* Allocate memory for the per-device structure */
        kp_devp = kmalloc(sizeof(struct kp_dev), GFP_KERNEL);

        if (!kp_devp) {
                printk(KERN_INFO "RB :Bad Kmalloc\n"); return -ENOMEM;
        }

        /* Request I/O region */
        sprintf(kp_devp->name, DEVICE_NAME);

        /* Connect the file operations with the cdev */
        cdev_init(&kp_devp->cdev, &kp_fops);
        kp_devp->cdev.owner = THIS_MODULE;

        /* Connect the major/minor number to the cdev */
        ret = cdev_add(&kp_devp->cdev, (kp_dev_number), 1);

        if (ret) {
                printk("Bad cdev\n");
                return ret;
        }

        /* Send uevents to udev, so it'll create /dev nodes */
        kp_dev_device = device_create(kp_dev_data, NULL, MKDEV(MAJOR(kp_dev_number), 10), NULL, DEVICE_NAME);
	
	printk(KERN_INFO "RBPROBE module inserted\n ");

	
        return 0;
}

void kprobe_extit(void)
{
   	
	unregister_kprobe(&kp_kpt);
	
	 /* Release the major number */
        unregister_chrdev_region((kp_dev_number), 1);

        /* Destroy device */
        device_destroy (kp_dev_data, MKDEV(MAJOR(kp_dev_number), 10));
        cdev_del(&kp_devp->cdev);
        kfree(kp_devp);

        /* Destroy driver_class */
        class_destroy(kp_dev_data);

    	printk(KERN_INFO "RBPROBE module removed\n ");
} 

module_init(kprobe_init);
module_exit(kprobe_extit);
MODULE_LICENSE("GPL"); /* You can't link the Kprobes API
                          unless your user module is GPL'ed */

