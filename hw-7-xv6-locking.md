# Homework: xv6 locking

Make sure you understand what would happen if the xv6 kernel executed the following code snippet:

```c
  struct spinlock lk;
  initlock(&lk, "test lock");
  acquire(&lk);
  acquire(&lk);
```

Explain in one sentence what happens.

Dead lock.



Explain in a few sentences why the kernel panicked. You may find it useful to look up the stack trace (the sequence of `%eip` values printed by `panic`) in the `kernel.asm`listing.

Function `iderw()` acquire `idelock ` in its procdure,  at the same time a `IRQ_TIMER` interrupt occur, `trap()` function call the `yield()` function which acquire `ptable.lock` , this make depth of pushcli nesting on same cpu more than one and cause the `sched lock` panic.



Explain in a few sentences why the kernel didn't panic. Why do `file_table_lock` and `ide_lock` have different behavior in this respect?

Function `filealloc` hold `ftable.lock` for very short times, much shorter than a timer interrupt cycle.



Why does `release()` clear `lk->pcs[0]` and `lk->cpu` *before* clearing `lk->locked`? Why not wait until after?

These debugging fields are protected by the lock and must only be edited while holding the lock.

