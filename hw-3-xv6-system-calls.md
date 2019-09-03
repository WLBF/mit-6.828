# Homework: xv6 system calls

## Part One: System call tracing

[https://github.com/WLBF/xv6-public/commit/59aac6f9a4f612a1b58c93e4d67f92324b4afe36](https://github.com/WLBF/xv6-public/commit/59aac6f9a4f612a1b58c93e4d67f92324b4afe36)

```c
// syscall.c

char calls[21][6] = {
  "fork",
  "exit",
  "wait",
  "pipe",
  "read",
  "kill",
  "exec",
  "fstat",
  "chdir",
  "dup",
  "getpid",
  "sbrk",
  "sleep",
  "uptime",
  "open",
  "write",
  "mknod",
  "unlink",
  "link",
  "mkdir",
  "close"
};

void
syscall(void)
{
  int num;
  uint ret;
  struct proc *curproc = myproc();

  num = curproc->tf->eax;
  if(num > 0 && num < NELEM(syscalls) && syscalls[num]) {
    ret = syscalls[num]();
    cprintf("%s -> %d\n", calls[num - 1], ret);
    curproc->tf->eax = ret;
  } else {
    cprintf("%d %s: unknown sys call %d\n",
            curproc->pid, curproc->name, num);
    curproc->tf->eax = -1;
  }
}
```

## Part Two: Date system call

```c
// syscall.c

#include "types.h"
#include "user.h"
#include "date.h"

int
main(int argc, char *argv[])
{
  struct rtcdate r;

  if (date(&r)) {
    printf(2, "date failed\n");
    exit();
  }

  printf(1, "%d-%d-%d-%d-%d-%d\n", r.year, r.month, r.day, r.hour, r.minute, r.second);

  exit();
}

```

```c
// syscall.c

int
sys_date(void)
{
  struct rtcdate* r;
  if(argptr(0, (void*)&r, sizeof(*r)) < 0)
    return -1;
  cmostime(r);
  return 0;
}
```

In order to make your new `date` program available to run from the xv6 shell, add `_date` to the `UPROGS` definition in `Makefile`.

Your strategy for making a date system call should be to clone all of the pieces of code that are specific to some existing system call, for example the "uptime" system call. You should grep for uptime in all the source files, using `grep -n uptime *.[chS]`.