# Lab 4: Preemptive Multitasking

**Question**

1. Compare `kern/mpentry.S` side by side with `boot/boot.S`. Bearing in mind that `kern/mpentry.S` is compiled and linked to run above `KERNBASE` just like everything else in the kernel, what is the purpose of macro `MPBOOTPHYS`? Why is it necessary in `kern/mpentry.S` but not in `boot/boot.S`? In other words, what could go wrong if it were omitted in `kern/mpentry.S`? 
   Hint: recall the differences between the link address and the load address that we have discussed in Lab 1.

   ```c
   #define MPBOOTPHYS(s) ((s) - mpentry_start + MPENTRY_PADDR)
   ```

   Before running, `mpentry.S`  will be move to `MPENTRY_PADDR`. However the link address of `mpentry.S` is based on `KERNBASE`, so we need macro `MPBOOTPHYS` map link address to new address start from `MPENTRY_PADDR`.
   
2. It seems that using the big kernel lock guarantees that only one CPU can run the kernel code at a time. Why do we still need separate kernel stacks for each CPU? Describe a scenario in which using a shared kernel stack will go wrong, even with the protection of the big kernel lock.

   Separate kernel stacks keep each CPU's function calls consist. If using a shared kernel stack one CPU's function call may return to another CPU's function call. This makes executing flow messed up.

3. In your implementation of `env_run()` you should have called `lcr3()`. Before and after the call to `lcr3()`, your code makes references (at least it should) to the variable `e`, the argument to `env_run`. Upon loading the `%cr3` register, the addressing context used by the MMU is instantly changed. But a virtual address (namely `e`) has meaning relative to a given address context--the address context specifies the physical address to which the virtual address maps. Why can the pointer `e` be dereferenced both before and after the addressing switch?

   Because in both `mem_init()` and `env_setup_vm()` the `envs` is mapped to array read-only by the user at linear address `UENVS`. The `e` is mapped to same physical address in both address context.

4. Whenever the kernel switches from one environment to another, it must ensure the old environment's registers are saved so they can be restored properly later. Why? Where does this happen?

   To restore environments we need the information about registers like `esp`  and `eip`.  It was done at `_alltraps` in `trapentry.S`. 