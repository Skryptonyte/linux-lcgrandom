#include <linux/init.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/string.h>  
#include <linux/time.h>

MODULE_LICENSE("GPL");

static struct proc_dir_entry *procfile;

static unsigned long long int a = 25214903917;
static unsigned long long int c = 11;
static unsigned long long int x = 0;
static unsigned long long int M = 1ull << 48;
static ssize_t lcg_read(struct file *file, char __user *ubuf,size_t count, loff_t *ppos)
{
    char message[30];
    int msglen = 0;
    if (*ppos > 0)
    {
        return 0;
    }
    //printk(KERN_INFO "User buffer: %p, Offset: %lld\n",ubuf,*ppos);
    // printk(KERN_INFO "Reading from LCG!\n");

    x = (x*a + c) % M;

    sprintf(message,"%lld\n",x);
    // printk(KERN_INFO "Random number: %lld\n",x);
    
    msglen = strlen(message);

    if (copy_to_user(ubuf, message,msglen))
    {
        printk(KERN_INFO "lcgrandom: failed to copy to userspace!");
        return -EFAULT;
    }
    printk(KERN_INFO "lcgrandom: Random number generated!");
    *ppos = 1;
    return msglen;
}
static struct proc_ops lcgFOP = 
{
    .proc_read = lcg_read
};
static int lcgInit(void)
{
    printk(KERN_INFO "lcgrandom: Starting LCG proc device");
    procfile = proc_create("lcgrandom",0666, NULL, &lcgFOP);

    printk(KERN_INFO "lcgrandom: Seeding LCG!");
    x = ktime_get_real();
    return 0;
}

static void lcgCleanup(void)
{

    printk(KERN_INFO "lcgrandom: Stopping LCG proc device");
    proc_remove(procfile);
}

module_init(lcgInit);
module_exit(lcgCleanup);