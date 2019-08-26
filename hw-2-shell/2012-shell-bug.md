# 2012 Shell Bug

 2012年官方提供的 sh.c 中存在一个 bug，简化后的代码如下： 

```c
int
getcmd(char *buf, int nbuf)
{
  memset(buf, 0, nbuf);
  fgets(buf, nbuf, stdin);
  if(buf[0] == 0) // EOF
    return -1;
  return 0;
}

int
fork1(void)
{
  int pid;
  
  pid = fork();
  if(pid == -1)
    perror("fork");
  return pid;
}

int
main(void)
{
  static char buf[100];
  int fd, r;

  while(getcmd(buf, sizeof(buf)) >= 0){
    printf("%s\n", buf);
    // fflush(stdin);
    if(fork1() == 0)
      exit(0);
    wait(&r);
  }
  exit(0);
}
```

在 Ubuntu 18.04 上编译之后通过重定向运行 shell 会出现输入 infinite loop 的现象：

```plain
$ a.out < t.sh
ls > y

cat < y | sort | uniq | wc > y1

cat y1

rm y1

ls | sort | uniq | wc

rm y

 > y

cat < y | sort | uniq | wc > y1

cat y1

rm y1

ls | sort | uniq | wc

rm y

 > y
...
```

这个问题实际上是使用 fork 时没有处理好 file descriptor  造成的，在 Unix 上不会出现，只会在 Linux 上出现，也解释了这个 bug 为什么会存在（因为都在用 mac ？）。
Fix:

``` c
int
main(void)
{
  static char buf[100];
  int fd, r;

  while(getcmd(buf, sizeof(buf)) >= 0){
    printf("%s\n", buf);
    fflush(stdin); // prepare for a fork()
    if(fork1() == 0)
      exit(0);
    wait(&r);
  }
  exit(0);
}
```



到了2018年，的版本可以发现，官方也作出了修正，将所有子进程中的 exit() 全部替换成了 _exit() 。
 <https://stackoverflow.com/questions/5422831/what-is-the-difference-between-using-exit-exit-in-a-conventional-linux-fo> 

 stackoverflow 上的类似问题：
 <https://stackoverflow.com/questions/50110992/why-does-forking-my-process-cause-the-file-to-be-read-infinitely>
 There's an open file description and an open file descriptor behind each stream. When the process forks, the child has its own set of open file descriptors (and file streams), but each file descriptor in the child shares the open file description with the parent. If the child process closing the file descriptors first did the equivalent of *lseek(fd, ?, SEEK_SET)*, then that would also position the file descriptor for the parent process, and that could lead to an infinite loop.  

 <https://en.wikipedia.org/wiki/POSIX> 

 [POSIX Definitions](http://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap03.html#tag_03_258)
 *3.165 File Description*
 See Open File Description in Open File Description. 

 *3.166 File Descriptor*
 A per-process unique, non-negative integer used to identify an open file for the purpose of file access. The value of a newly-created file descriptor is from zero to {OPEN_MAX}-1. A file descriptor can have a value greater than or equal to {OPEN_MAX} if the value of {OPEN_MAX} has decreased (see sysconf) since the file descriptor was opened. File descriptors may also be used to implement message catalog descriptors and directory streams; see also Open File Description. 

 *3.258 Open File Description*
 A record of how a process or group of processes is accessing a file. Each file descriptor refers to exactly one open file description, but an open file description can be referred to by more than one file descriptor. The file offset, file status, and file access modes are attributes of an open file description. 

 [POSIX open()](http://pubs.opengroup.org/onlinepubs/9699919799/functions/open.html)
 The *open()* function shall establish the connection between a file and a file descriptor. It shall create an open file description that refers to a file and a file descriptor that refers to that open file description.
 The *open()* function shall return a file descriptor for the named file, allocated as described in File Descriptor Allocation. The open file description is new, and therefore the file descriptor shall not share it with any other process in the system.  

  [POSIX fork()](http://pubs.opengroup.org/onlinepubs/9699919799/functions/fork.html)
 The child process shall have its own copy of the parent's file descriptors. Each of the child's file descriptors shall refer to the same open file description with the corresponding file descriptor of the parent. 

 [POSIX fflush()](http://pubs.opengroup.org/onlinepubs/9699919799/functions/fflush.html)
 For a stream open for reading with an underlying file description, if the file is not already at EOF, and the file is one capable of seeking, the file offset of the underlying open file description shall be set to the file position of the stream, and any characters pushed back onto the stream by ungetc() or ungetwc() that have not subsequently been read from the stream shall be discarded (without further changing the file offset). [Option End] 

 If stream is a null pointer, fflush() shall perform this flushing action on all streams for which the behavior is defined above. 

 [GLIBC Bug 23151](https://sourceware.org/bugzilla/show_bug.cgi?id=23151) 

 [2.5.1 Interaction of File Descriptors and Standard I/O Streams](http://pubs.opengroup.org/onlinepubs/9699919799/functions/V2_chap02.html#tag_15_05_01) 