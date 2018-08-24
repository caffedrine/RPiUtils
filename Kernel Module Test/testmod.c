#include <linux/init.h>             // Macros used to mark up functions e.g., __init __exit
#include <linux/module.h>           // Core header for loading LKMs into the kernel
#include <linux/kernel.h>           // Contains types, macros, functions for the kernel
 
MODULE_LICENSE("GPL");              ///< The license type -- this affects runtime behavior
MODULE_AUTHOR("Mihai Eminescu");      ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("Modul de kernel compus de Eminescu insasi ^.^");  ///< The description -- see modinfo
MODULE_VERSION("1.3.3.7");              ///< The version of the module
 
static char *name = "world";        ///< An example LKM argument -- default value is "world"
module_param(name, charp, S_IRUGO); ///< Param desc. charp = char ptr, S_IRUGO can be read/not changed
MODULE_PARM_DESC(name, "The name to display in /var/log/kern.log");  ///< parameter description
 

static int __init testmod_init(void)
{
   	printk(KERN_INFO "EBB: Hello %s from the TESTMOD LKM!\n", name);
   	return 0;
}
 

static void __exit testmod_exit(void)
{
	printk(KERN_INFO "EBB: Goodbye %s from the TESTMOD LKM!\n", name);
}
 
module_init(testmod_init);
module_exit(testmod_exit);