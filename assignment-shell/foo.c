#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>

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
