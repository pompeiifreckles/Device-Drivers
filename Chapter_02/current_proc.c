#include<linux/module.h>
#include<linux/init.h>
#include<asm/current.h>
#include<linux/sched.h>

MODULE_LICENSE("Dual BSD/GPL");

static int hello_init(void) {
  printk(KERN_ALERT "Hello, World\n");
  printk(KERN_ALERT "The process is %s (pid %i)\n", current->comm,\
      current->pid);
  return 0;
}

static void hello_exit(void) {
  printk(KERN_ALERT "Goodbye, Cruel World\n");
}

module_init(hello_init);
module_exit(hello_exit);
