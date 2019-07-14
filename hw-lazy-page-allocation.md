# Homework: xv6 lazy page allocation

## Part One: Eliminate allocation from sbrk()

Page fault occurs because shell calls `malloc` system call in `runcmd` function, which calls `sbrk` system call underground.

## Part Two: Lazy allocation

```c
  case T_PGFLT:
    a = rcr2();
    a = PGROUNDDOWN(a);
    mem = kalloc();
    memset(mem, 0, PGSIZE);
    mappages(myproc()->pgdir, (char*)a, PGSIZE, V2P(mem), PTE_W|PTE_U);
    break;
```

