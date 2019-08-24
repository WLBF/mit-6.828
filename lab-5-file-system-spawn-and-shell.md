# Lab 5: File system, Spawn and Shell

**Question**

1. Do you have to do anything else to ensure that this I/O privilege setting is saved and restored properly when you subsequently switch from one environment to another? Why?

   Processor pushed `eflags` value to kernel stack at trap, the register will be restored by `iret` when kernel try to rerun the environment.
   
2.  Think about what you would have to do in order to implement `exec` in user space, and be sure you understand why it is harder.

    `exec` need to load image and setup frame while `exec` self is still running.

