# Building and Running Modules 

## kernel modules vs applications
  most small and medium sized applications perform a single task from beginning to end
  every kernel module just registers itself to serve future requests
  not every application is event-driven but every kernel module is
  Application that terminates can be lazy in releasing resources or avoid cleanup altogether
  the exit function of a module must carefully undo everything the init function built up
  difference in fault handling
   - a segmentation fault may be harmless during application development
   - a kernel fault kills the process atleast, if not the whole system
  Applications are laid out in virtual memory with a very large stack area
  Kernel instead has a very small stack, as small as a single 4096 byte page
  (Thus it is never good idea to declare large automatic variables, instead dynamically allocate at call time)
  Kernel code cannot do floating point arithmatic (since kernel does not need floating point, the extra overhead is not worthwhile)

## kernel space vs user space
  The operating system must account for independent operation of programs and protection against unauthorized access to resources.
  This nontrivial task is possible only if the CPU enforces protection of system software from the applications.
  Every modern processor is able to enforce this behavior. The chosen approach is to implement different operating modalities in CPU itself.
  Unix systems are designed to take advantage of this hardware feature using two such levels.
  Under Unix, kernel executes in the highest level (also called supervisor mode), where everything is allowed.
  whereas applications execute at the lowest level (the so-called user mode), where the processor regulates the direct access to hardware and unauthorized access to memory.

## Makefile

``` Makefile
  obj-m := hello.o        // single file

  obj-m := hello.o        // multiple files
  module-objs := file1.o file2.o

  # make that can be invoked directly from the cli, this might become tiresome
  # make -C /path/to/src/tree M=`pwd` modules

  # If KERNELRELEASE is defined, we've been invoked from the
  # kernel build system and can use its language.
  ifneq ($(KERNELRELEASE),)
    obj-m := hello.o
  # Otherwise we were directly called from the command
  # line; invoke the kernel build system
  else

    KERNELDIR ?= /lib/modules/$(shell uname -r)/build
    PWD := $(shell pwd)

  default:
    $(MAKE) -C $(KERNELDIR) M=$(PWD) modules

  endif
```

## Current Process
  Kernel code can refer to the current process by accessing the global item current defined in <asm/current.h>,
  which yields a pointer to struct task_struct, defined in <linux/sched.h>

  printk(KERN_INFO "The process is \"%s\" (pid %i)\n", current->comm, current->pid);

  current->comm   base name of the program
  current->pid    process id

## insmod, rmmod, modprobe
  insmod: it relies on syscall the function sys_init_module from kernel/module.h
    allocates kernel memory with vmalloc
    copies modules text into that memory region.
    resolves kernel references in module via kernel symbol table.
    calls the module's initialization function to get everything going.

  modprobe: like insmod, loads a module into the kernel.
    It differs in taht int will look at the modules to be loaded to see whether it references
    any symbols that are not currently defined in the kernel.
    modprobe looks for other modules in the current module search path that defines the relevant symbols.
    it also loads those modules into the kernels as well.

  rmmod: removes the module after eaxecuting the cleanup function.
    fails if the modules is busy (example - module has opened a file)

## Version Dependency
  UTS_RELEASE: macro expands to string describing the version of the kernel. "2.6.10"
  LINUX_VERSION_CODE: returns version one byte per version release number. "2.6.10" -> 0x02060a (132618)
  KERNEL_VERSION(major, minor, release): returns kernel version in decimal. "2.6.10" -> 132618

## Exporting symbols
  EXPORT_SYMBOL(name);
  EXPORT_SYMBOL_GPL(name);    _GPL makes the symbol available to only GPL-licensed modules

## Preliminaries
  
  #include<linux/module.h>
  #include<linux/init.h>

  module.h  contains lots of symbols and functions for loadable modules
  init.h    contains initialization and cleanup function

  MODULE_LICENSE("GPL");

  specifies license identified by kernel. "GPL", "GPL v2", "GPL and additional rights", "Dual BSD/GPL", "Dual MPL/GPL" and "Proprietary"(default)

## Initialization and Shutdown
  static int __init initialization_function(void) {
    /* Initialization code here */
  }
  
  module_init(initialization_function);

  __init (and __initdata) are optional but worth the effort. The module loader drops the intialization function (and data structures) and clears the memory after its loaded.
  __devinit (and __devinitdata) are for kernels not configured for hotpluggable devices

## Cleanup Function
  
  static void __exit cleanup_function(void) {
    /* Cleanup code goes here */
  }

  module_exit(cleanup_function);

  The cleanup function has no value to return, so it is declared void
  __exit modifier marks for unload only. If module is built directly into kernel it is discarded
  __exit functions are called only at the time of unload and shutdown, any other use is an error

## Module Parameters: moduleparam.h
  
  _**insmod hellop howmany=10 whom="Mom"**_

    static char *whom = "world";
    static int howmany = 1;
    module_param(howmany, int, S_IRUGO);
    module_param(whom, charp, S_IRUGO);

   **Data Types:**
   bool, invbool
   charp
   int
   long
   short
   uint
   ulong
   ushort
    
   **Array parameters:**
   ``` C
   module_param_array(name, type, num, perm);
   ```
   name - name of the array
   num - number of elements

   **permission: <linux/stat.h>**
   This value controls who can access the representation of module parameter in sysfs
   perm is set to 0, there is nos sysfs entry at all; otherwise appears in /sys/module
   S_IRUGO           read-only
   S_IRUGO|S_IWUSR   read and write

   NOTE: you should not make module parameters writable, unless you are prepared to detect the change and react accordingly.