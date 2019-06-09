# Lab 1: Booting a PC

**Exercise 1.** Familiarize yourself with the assembly language materials available on [the 6.828 reference page](https://pdos.csail.mit.edu/6.828/2018/reference.html). You don't have to read them now, but you'll almost certainly want to refer to some of this material when reading and writing x86 assembly.

We do recommend reading the section "The Syntax" in [Brennan's Guide to Inline Assembly](http://www.delorie.com/djgpp/doc/brennan/brennan_att_inline_djgpp.html). It gives a good (and quite brief) description of the AT&T assembly syntax we'll be using with the GNU assembler in JOS.



**Exercise 2.** Use GDB's si (Step Instruction) command to trace into the ROM BIOS for a few more instructions, and try to guess what it might be doing. You might want to look at[Phil Storrs I/O Ports Description](http://web.archive.org/web/20040404164813/members.iweb.net.au/~pstorr/pcbook/book2/book2.htm), as well as other materials on the [6.828 reference materials page](https://pdos.csail.mit.edu/6.828/2018/reference.html). No need to figure out all the details - just the general idea of what the BIOS is doing first.

```
# long jump instruction.
[f000:fff0]    0xffff0: ljmp   $0xf000,$0xe05b

[f000:e05b]    0xfe05b: cmpl   $0x0,%cs:0x6ac8                                           
[f000:e062]    0xfe062: jne    0xfd2e1

# Set up initial stack to 0x0:0x7000
[f000:e066]    0xfe066: xor    %dx,%dx 
[f000:e068]    0xfe068: mov    %dx,%ss
[f000:e06a]    0xfe06a: mov    $0x7000,%esp


[f000:e070]    0xfe070: mov    $0xf34c2,%edx
[f000:e076]    0xfe076: jmp    0xfd15c
[f000:d15c]    0xfd15c: mov    %eax,%ecx

# set processor to ignore maskable external interrupts.
[f000:d15f]    0xfd15f: cli

# string operations increment
[f000:d160]    0xfd160: cld

# access CMOS-RAM
[f000:d161]    0xfd161: mov    $0x8f,%eax
[f000:d167]    0xfd167: out    %al,$0x70
[f000:d169]    0xfd169: in     $0x71,%al

# enable the A20 line
[f000:d16b]    0xfd16b: in     $0x92,%al
[f000:d16d]    0xfd16d: or     $0x2,%al
[f000:d16f]    0xfd16f: out    %al,$0x92

# load descriptor tables
[f000:d171]    0xfd171: lidtw  %cs:0x6ab8
[f000:d177]    0xfd177: lgdtw  %cs:0x6a74

# enter Protected Mode
[f000:d17d]    0xfd17d: mov    %cr0,%eax
[f000:d180]    0xfd180: or     $0x1,%eax
[f000:d184]    0xfd184: mov    %eax,%cr0

# Jump to next instruction, but in 32-bit code segment.
# Switches processor into 32-bit mode.
[f000:d187]    0xfd187: ljmpl  $0x8,$0xfd18f

# set data segments
=> 0xfd18f:     mov    $0x10,%eax
=> 0xfd194:     mov    %eax,%ds
=> 0xfd196:     mov    %eax,%es
=> 0xfd198:     mov    %eax,%ss
=> 0xfd19a:     mov    %eax,%fs
=> 0xfd19c:     mov    %eax,%gs

# unknown
=> 0xfd19e:     mov    %ecx,%eax
=> 0xfd1a0:     jmp    *%edx
=> 0xf34c2:     push   %ebx
=> 0xf34c3:     sub    $0x2c,%esp
=> 0xf34c6:     movl   $0xf5b5c,0x4(%esp)
=> 0xf34ce:     movl   $0xf447b,(%esp)
=> 0xf34d5:     call   0xf099e
=> 0xf099e:     lea    0x8(%esp),%ecx
=> 0xf09a2:     mov    0x4(%esp),%edx
=> 0xf09a6:     mov    $0xf5b58,%eax
=> 0xf09ab:     call   0xf0574
...

```



When the BIOS runs, it sets up an interrupt descriptor table and initializes various devices such as the VGA display. This is where the "`Starting SeaBIOS`" message you see in the QEMU window comes from.

After initializing the PCI bus and all the important devices the BIOS knows about, it searches for a bootable device such as a floppy, hard drive, or CD-ROM. Eventually, when it finds a bootable disk, the BIOS reads the *boot loader* from the disk and transfers control to it.



**Exercise 3.** Be able to answer the following questions:

- At what point does the processor start executing 32-bit code? What exactly causes the switch from 16- to 32-bit mode?
```
     # Jump to next instruction, but in 32-bit code segment.
     # Switches processor into 32-bit mode.
     ljmp    $PROT_MODE_CSEG, $protcseg
```
- What is the *last* instruction of the boot loader executed, and what is the *first* instruction of the kernel it just loaded?
- *Where* is the first instruction of the kernel?
```
   => 0x7d6b:      call   *0x10018
   --- kernel ---
   => 0x10000c:    movw   $0x1234,0x472
```
- How does the boot loader decide how many sectors it must read in order to fetch the entire kernel from disk? Where does it find this information?  
read ELFHDR from 0x10000, ELFHDR->e_phnum indicate the num of sectors need to be load.
```
   #define SECTSIZE	512
   #define ELFHDR		((struct Elf *) 0x10000) // scratch space
   
   void readsect(void*, uint32_t);
   void readseg(uint32_t, uint32_t, uint32_t);
   
   void
   bootmain(void)
   {
   	struct Proghdr *ph, *eph;
   
   	// read 1st page off disk
   	readseg((uint32_t) ELFHDR, SECTSIZE*8, 0);
   
   	// is this a valid ELF?
   	if (ELFHDR->e_magic != ELF_MAGIC)
   		goto bad;
   
   	// load each program segment (ignores ph flags)
   	ph = (struct Proghdr *) ((uint8_t *) ELFHDR + ELFHDR->e_phoff);
   	eph = ph + ELFHDR->e_phnum;
   	for (; ph < eph; ph++)
   		// p_pa is the load address of this segment (as well
   		// as the physical address)
   		readseg(ph->p_pa, ph->p_memsz, ph->p_offset);
   
   	// call the entry point from the ELF header
   	// note: does not return!
   	((void (*)(void)) (ELFHDR->e_entry))();
   
   bad:
   	outw(0x8A00, 0x8A00);
   	outw(0x8A00, 0x8E00);
   	while (1)
   		/* do nothing */;
   }
```



**Exercise 9.** Determine where the kernel initializes its stack, and exactly where in memory its stack is located. How does the kernel reserve space for its stack? And at which "end" of this reserved area is the stack pointer initialized to point to?

   ```
   ...
   .data
   ###################################################################
   # boot stack
   ###################################################################
   	.p2align	PGSHIFT		# force page alignment
   	.globl		bootstack
   bootstack:
   	.space		KSTKSIZE
   	.globl		bootstacktop   
   bootstacktop:
   ```

   ```
   	# Clear the frame pointer register (EBP)
   	# so that once we get into debugging C code,
   	# stack backtraces will be terminated properly.
   	movl	$0x0,%ebp			# nuke frame pointer
   
   	# Set the stack pointer
   	movl	$(bootstacktop),%es
   ```

   ```
   	# Set the stack pointer
   	movl	$(bootstacktop),%esp
   f0100034:	bc 00 30 11 f0       	mov    $0xf0113000,%esp
   ```



**Exercise 12.** Modify your stack backtrace function to display, for each `eip`, the function name, source file name, and line number corresponding to that `eip`.

In `debuginfo_eip`, where do `__STAB_*` come from? This question has a long answer; to help you to discover the answer, here are some things you might want to do:

- look in the file `kern/kernel.ld` for `__STAB_*`
- run objdump -h obj/kern/kernel
- run objdump -G obj/kern/kernel
- run gcc -pipe -nostdinc -O2 -fno-builtin -I. -MD -Wall -Wno-format -DJOS_KERNEL -gstabs -c -S kern/init.c, and look at init.s.
- see if the bootloader loads the symbol table in memory as part of loading the kernel binary

```
	/* Include debugging information in kernel memory */
	.stab : {
		PROVIDE(__STAB_BEGIN__ = .);
		*(.stab);
		PROVIDE(__STAB_END__ = .);
		BYTE(0)		/* Force the linker to allocate space
				   for this section */
	}

	.stabstr : {
		PROVIDE(__STABSTR_BEGIN__ = .);
		*(.stabstr);
		PROVIDE(__STABSTR_END__ = .);
		BYTE(0)		/* Force the linker to allocate space
				   for this section */
	}
```

```
  2 .stab         00003d15  f01022ac  001022ac  000032ac  2**2
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  3 .stabstr      00001989  f0105fc1  00105fc1  00006fc1  2**0
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
```

