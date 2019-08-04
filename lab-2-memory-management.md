# Lab 2: Memory Management

## Part 1: Physical Page Management

```
0x10000000 (256MB)  +--------------------------------+
                    |                                |\
                    |          free pages            | |
                    |                                | |
                    |                                | |
                    +--------------------------------+ |
                    |     struct PageInfo array      | |--- extend memory
                    +--------------------------------+ |
                    |     page directory (1 page)    | |
                    +--------------------------------+ |
                    |                                | |
                    |      kernel code and data      | |
                    |                                |/
0x00100000 (1MB)    +--------------------------------+
                    |           IO hole              |
0x000a0000 (640KB)  +--------------------------------+
                    |                                |\
                    |          free pages            | |
                    |                                | |
                    |--------------------------------| |--- base memory
                    |real-mode IDT & BIOS structures | |
                    |           (1 page)             |/
0x00000000          +--------------------------------+
```

## Part 2: Virtual Memory

### Virtual, Linear, and Physical Addresses

```

           Selector  +--------------+         +-----------+
          ---------->|              |         |           |
                     | Segmentation |         |  Paging   |
Software             |              |-------->|           |---------->  RAM
            Offset   |  Mechanism   |         | Mechanism |
          ---------->|              |         |           |
                     +--------------+         +-----------+
            Virtual                   Linear                Physical
```

### Page Translation

[5.2 Page Translation](https://pdos.csail.mit.edu/6.828/2018/readings/i386/s05_02.htm)

[6.4 Page-Level Protection](https://pdos.csail.mit.edu/6.828/2018/readings/i386/s06_04.htm)

### Question

1. What entries (rows) in the page directory have been filled in at this point? What addresses do they map and where do they point? In other words, fill out this table as much as possible:

   | Entry | Base Virtual Address | Points to (logically):                                     |
   | ----- | -------------------- | ---------------------------------------------------------- |
   | 1023  | 0xffffffff           | Page table for top 4MB of phys memory                      |
   | ~     | ~                    | ~                                                          |
   | 960   | 0xf0000000           | Page table for the first 4MB of phys memory                |
   | 959   | 0xefff8000           | Kernel stack                                               |
   | .     |                      |                                                            |
   | 957   | 0xef7bd000(UVPT)     | Page directory itself                                      |
   | 956   | 0xef000000(UPAGES)   | Map 'pages' read-only by the user at linear address UPAGES |
   |       |                      |                                                            |
   | .     | ?                    | ?                                                          |
   | 2     | 0x00800000           | ?                                                          |
   | 1     | 0x00400000           | ?                                                          |
   | 0     | 0x00000000           | [see next question]                                        |

2. We have placed the kernel and user environment in the same address space. Why will user programs not be able to read or write the kernel's memory? What specific mechanisms protect the kernel memory?

   page level protection

3. What is the maximum amount of physical memory that this operating system can support? Why?

   pde_num * pte_num = 2^10 * 2^10 = 2^20 pages = 2^32 bytes

4. How much space overhead is there for managing memory, if we actually had the maximum amount of physical memory? How is this overhead broken down?

   sizeof(struct PageInfo) * page_num + pde_page + pte_pages = 64 * 2^20 + 1024 * 32 + 2^20 * 32

5. Revisit the page table setup in `kern/entry.S` and `kern/entrypgdir.c`. Immediately after we turn on paging, EIP is still a low number (a little over 1MB). At what point do we transition to running at an EIP above KERNBASE? What makes it possible for us to continue executing at a low EIP between when we enable paging and when we begin running at an EIP above KERNBASE? Why is this transition necessary?

   ```asm
   	mov	$relocated, %eax
   	jmp	*%eax
   relocated:
   ```

   virtual address [0, 4MB) and [KERNBASE, KERNBASE+4MB) both are map to physical address [0, 4MB)





