#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <asm/io.h>
#include <linux/ioport.h>
#include <asm/errno.h>
#include <linux/uaccess.h>

MODULE_INFO(intree, "Y");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Filip Wojda");
MODULE_DESCRIPTION("Kernel module for SYKOM project");
MODULE_VERSION("0.01");

#define SYKT_GPIO_BASE_ADDR (0x00100000)
#define SYKT_GPIO_SIZE (0x8000)
#define SYKT_EXIT (0x3333)
#define SYKT_EXIT_CODE (0x7F)

#define SYKT_GPIO_ADDR_SPACE (baseptr)
#define SYKT_GPIO_A_ADDR_OFFSET (0x258)
#define SYKT_GPIO_S_ADDR_OFFSET (0x270)
#define SYKT_GPIO_W_ADDR_OFFSET (0x268)

#define PROC_DIR_NAME "proj4wojfil"
#define PROC_FILE_REJA "rejA"
#define PROC_FILE_REJS "rejS"
#define PROC_FILE_REJW "rejW"

void __iomem *baseptr;

void __iomem *baseptrA;
void __iomem *baseptrS;
void __iomem *baseptrW;

static struct proc_dir_entry *rejA;
static struct proc_dir_entry *rejS;
static struct proc_dir_entry *rejW;

// odczyt, zapis na odpowiednie miejsce w pamieci
static ssize_t rejA_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos) {
    unsigned int temp;
    char kernel_buffer[20];
    
    if(copy_from_user(kernel_buffer, buf, min(count, sizeof(kernel_buffer)))){
	return -EFAULT;
    }

    sscanf(kernel_buffer, "%o", &temp);
    writel(temp, baseptrA);
    return count;
}

static ssize_t rejS_read(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
    //unsigned int temp = 50; 
    unsigned int temp = readl(baseptrS);
    char kernel_buffer[200];

    if (*ppos >= sizeof(kernel_buffer))
        return 0;

    int len = snprintf(kernel_buffer, sizeof(kernel_buffer), "%u\n", temp);

    if (len < 0) {
        return -EFAULT;
    }

    size_t to_copy = min(len - *ppos, count);

    if (copy_to_user(buf, kernel_buffer + *ppos, to_copy)) {
        return -EFAULT;
    }

    *ppos += to_copy;

    return to_copy;
}


static ssize_t rejW_read(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
    unsigned int temp = readl(baseptrW);
    char kernel_buffer[200];

    if (*ppos >= sizeof(kernel_buffer))
        return 0;

    int len = snprintf(kernel_buffer, sizeof(kernel_buffer), "%u\n", temp);

    if (len < 0) {
        return -EFAULT;
    }

    size_t to_copy = min(len - *ppos, count);

    if (copy_to_user(buf, kernel_buffer + *ppos, to_copy)) {
        return -EFAULT;
    }

    *ppos += to_copy;

    return to_copy;
}

//makra
static const struct file_operations proc_rejA_fops = 
{
    .owner = THIS_MODULE,
    .write = rejA_write,
};

static const struct file_operations proc_rejS_fops = 
{
    .owner = THIS_MODULE,
    .read = rejS_read,
};

static const struct file_operations proc_rejW_fops = 
{
    .owner = THIS_MODULE,
    .read = rejW_read,
};

int my_init_module(void) 
{
    static struct proc_dir_entry *proj4wojfil;

    printk(KERN_INFO "Init my module.\n");
    baseptr = ioremap(SYKT_GPIO_BASE_ADDR, SYKT_GPIO_SIZE);
    if (!baseptr) {
        printk(KERN_ERR "Failed to remap memory for baseptr\n");
        return -ENOMEM;
    }

    baseptrA = ioremap(SYKT_GPIO_BASE_ADDR + SYKT_GPIO_A_ADDR_OFFSET, 8);
    if (!baseptrA) {
        printk(KERN_ERR "Failed to remap memory for baseptrA\n");
        iounmap(baseptr);
        return -ENOMEM;
    }

    baseptrS = ioremap(SYKT_GPIO_BASE_ADDR + SYKT_GPIO_S_ADDR_OFFSET, 8);
    if (!baseptrS) {
        printk(KERN_ERR "Failed to remap memory for baseptrS\n");
        iounmap(baseptrA);
        iounmap(baseptr);
        return -ENOMEM;
    }

    baseptrW = ioremap(SYKT_GPIO_BASE_ADDR + SYKT_GPIO_W_ADDR_OFFSET, 8);
    if (!baseptrW) {
        printk(KERN_ERR "Failed to remap memory for baseptrW\n");
        iounmap(baseptrS);
        iounmap(baseptrA);
        iounmap(baseptr);
        return -ENOMEM;
    }

    proj4wojfil = proc_mkdir(PROC_DIR_NAME, NULL);
     if(!proj4wojfil){
        printk(KERN_INFO "Failed to create procfs directory\n");
        return -ENOMEM;
    }

    rejA = proc_create(PROC_FILE_REJA, 0666, proj4wojfil, &proc_rejA_fops);
    if (!rejA) {
        printk(KERN_ERR "Failed to create /proc/proj4wojfil/rejA\n");
        iounmap(baseptrW);
        iounmap(baseptrS);
        iounmap(baseptrA);
        iounmap(baseptr);
        return -ENOMEM;
    }

    rejS = proc_create(PROC_FILE_REJS, 0444, proj4wojfil, &proc_rejS_fops);
    if (!rejS) {
        printk(KERN_ERR "Failed to create /proc/proj4wojfil/rejS\n");
        remove_proc_entry("proj4wojfil/rejA", NULL);
        iounmap(baseptrW);
        iounmap(baseptrS);
        iounmap(baseptrA);
        iounmap(baseptr);
        return -ENOMEM;
    }

    rejW = proc_create(PROC_FILE_REJW, 0444, proj4wojfil, &proc_rejW_fops);
    if (!rejW) {
        printk(KERN_ERR "Failed to create /proc/proj4wojfil/rejW\n");
        remove_proc_entry("proj4wojfil/rejA", NULL);
        remove_proc_entry("proj4wojfil/rejS", NULL);
        iounmap(baseptrW);
        iounmap(baseptrS);
        iounmap(baseptrA);
        iounmap(baseptr);
        return -ENOMEM;
    }

    return 0;
}

void my_cleanup_module(void) 
{
    printk(KERN_INFO "Cleanup my module.\n");
    writel(SYKT_EXIT | ((SYKT_EXIT_CODE) << 16), baseptr);
    remove_proc_entry("proj4wojfil/rejA", NULL);
    remove_proc_entry("proj4wojfil/rejS", NULL);
    remove_proc_entry("proj4wojfil/rejW", NULL);
    remove_proc_entry("proj4wojfil", NULL);
    iounmap(baseptr);

    iounmap(baseptrA);
    iounmap(baseptrS);
    iounmap(baseptrW);
}

module_init(my_init_module);
module_exit(my_cleanup_module);