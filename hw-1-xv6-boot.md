# Homework boot xv6

## Finding and breaking at an address

```
$ nm kernel | grep _start
8010a48c D _binary_entryother_start
8010a460 D _binary_initcode_start
0010000c T _start
```

<https://en.wikipedia.org/wiki/Nm_(Unix)>

GNU **nm** lists the symbols from object files *objfile*.... If no object files are listed as arguments, **nm** assumes the file *a.out*.

<https://sourceware.org/gdb/current/onlinedocs/gdb/Specify-Location.html#Specify-Location>

```
(gdb) br * 0x0010000c
Breakpoint 1 at 0x10000c
(gdb) c
Continuing.
The target architecture is assumed to be i386
=> 0x10000c:	mov    %cr4,%eax
Breakpoint 1, 0x0010000c in ?? ()
(gdb)
```

## Exercise:

```
(gdb) info reg
eax            0x0      0
ecx            0x0      0
edx            0x1f0    496
ebx            0x10074  65652
esp            0x7bdc   0x7bdc
ebp            0x7bf8   0x7bf8
esi            0x10074  65652
edi            0x0      0
eip            0x10000c 0x10000c
eflags         0x46     [ PF ZF ]
cs             0x8      8
ss             0x10     16
ds             0x10     16
es             0x10     16
fs             0x0      0
gs             0x0      0
```

The `x/24x` command display (e**x**amine) 24 words at the top of the stack in hex.


```
(gdb) x/24x $esp
0x7bdc: 0x00007d8d      0x00000000      0x00000000      0x00000000
0x7bec: 0x00000000      0x00000000      0x00000000      0x00000000
0x7bfc: 0x00007c4d      0x8ec031fa      0x8ec08ed8      0xa864e4d0
0x7c0c: 0xb0fa7502      0xe464e6d1      0x7502a864      0xe6dfb0fa
0x7c1c: 0x16010f60      0x200f7c78      0xc88366c0      0xc0220f01
0x7c2c: 0x087c31ea      0x10b86600      0x8ed88e00      0x66d08ec0
```

### Which part of the stack printout is actually the stack?

After examine `bootblock.asm` and `bootasm.S` , we know that stack is start from `0x7c00`.

```asm
  # Set up the stack pointer and call into C.
  movl    $start, %esp
```

```asm
Disassembly of section .text:

00007c00 <start>:
# with %cs=0 %ip=7c00.

.code16                       # Assemble for 16-bit mode
.globl start
start:
  cli                         # BIOS enabled interrupts; disable
    7c00:	fa                   	cli  
...
```

stack:

```
0x7bdc: 0x00007d8d <--- current esp
0x7be0: 0x00000000
0x7be4: 0x00000000
0x7be8: 0x00000000
0x7bec: 0x00000000
0x7bf0: 0x00000000
0x7bf4: 0x00000000
0x7bf8: 0x00000000
0x7bfc: 0x00007c4d
------------------
0x7c00: 0x8ec031fa <--- init esp
```



### Single step through the call to `bootmain`; what is on the stack now?

```asm
  call    bootmain
    7c48:	e8 ee 00 00 00       	call   7d3b <bootmain>

  # If bootmain returns (it shouldn't), trigger a Bochs
  # breakpoint if running under Bochs, then loop.
  movw    $0x8a00, %ax            # 0x8a00 -> port 0x8a00
    7c4d:	66 b8 00 8a          	mov    $0x8a00,%a
  ...
```

Call `bootmain()` push the return address to the stack which is `0x7c4d`.

stack:

```
0x7bfc: 0x00007c4d <--- bootmain() return address
------------------
0x7c00: 0x8ec031fa <--- init esp
```

The prologue in `bootmain()` makes a stack frame

```asm
00007d3b <bootmain>:
{
    7d3b:	55                   	push   %ebp
    7d3c:	89 e5                	mov    %esp,%ebp
    7d3e:	57                   	push   %edi
    7d3f:	56                   	push   %esi
    7d40:	53                   	push   %ebx
    7d41:	83 ec 0c             	sub    $0xc,%esp
```

stack:

```
0x7bdc: 0x00007d8d <--- entry() return address
0x7be0: 0x00000000 \
0x7be4: 0x00000000  |-- local vars (sub    $0xc,%esp)
0x7be8: 0x00000000 /
0x7bec: 0x00000000 <--- old ebx
0x7bf0: 0x00000000 <--- old esi
0x7bf4: 0x00000000 <--- old edi
0x7bf8: 0x00000000 <--- old ebp
0x7bfc: 0x00007c4d <--- bootmain() return address
------------------
0x7c00: 0x8ec031fa <--- init esp
```



### What do the first assembly instructions of `bootmain` do to the stack? 

push `ebp` onto stack.

```asm
=> 0x7d3b:      push   %ebp
```

stack:

```
0x7bf8: 0x00000000
0x7bfc: 0x00007c4d
------------------
0x7c00: 0x8ec031fa <--- init esp
```



### Look for the call that changes `eip` to 0x10000c. What does that call do to the stack?

push return address of `entry()` onto stack.

```
(gdb) b * 0x7d87
Breakpoint 1 at 0x7d87
(gdb) c
Continuing.
The target architecture is assumed to be i386
=> 0x7d87:      call   *0x10018

Thread 1 hit Breakpoint 1, 0x00007d87 in ?? ()
(gdb) si
=> 0x10000c:    mov    %cr4,%eax
0x0010000c in ?? ()
```

```asm
  entry();
    7d87:	ff 15 18 00 01 00    	call   *0x10018
    7d8d:	eb d5                	jmp    7d64 <bootmain+0x29>
```

stack:

```
0x7bdc: 0x00007d8d <--- entry() return address
0x7be0: 0x00000000
0x7be4: 0x00000000
0x7be8: 0x00000000
0x7bec: 0x00000000
0x7bf0: 0x00000000
0x7bf4: 0x00000000
0x7bf8: 0x00000000
0x7bfc: 0x00007c4d
------------------
0x7c00: 0x8ec031fa <--- init esp
```
