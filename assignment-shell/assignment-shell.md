# Solution

## Executing simple commands

 <http://man7.org/linux/man-pages/man3/exec.3.html>

Special semantics for `execlp()` and `execvp()` The `execlp()`, `execvp()`, and `execvpe()` 

functions duplicate the actions of the shell in *searching for an executable file if the specified filename does not contain a slash (/) character*.  The file is sought in the colon-separated list of directory pathnames specified in the PATH environment variable.  If this variable isn't defined, the path list defaults to a list that includes the directories returned by `confstr(_CS_PATH) `(which typically returns the value "/bin:/usr/bin") and possibly also the current working directory; see NOTES for further details.

``` c
  switch(cmd->type){
  default:
    fprintf(stderr, "unknown runcmd\n");
    exit(-1);

  case ' ':
    ecmd = (struct execcmd*)cmd;
    if(ecmd->argv[0] == 0)
      exit(0);
    execvp(ecmd->argv[0], ecmd->argv);
    fprintf(stderr, "execv() %s error\n", ecmd->argv[0]);
    exit(-1);
    break
```

## I/O redirection

 <http://man7.org/linux/man-pages/man2/close.2.html>
 <http://man7.org/linux/man-pages/man3/open.3p.html>

 The open() function shall return a file descriptor for the named file that is the lowest file descriptor not currently open for that process. 

 <https://www.tldp.org/LDP/abs/html/io-redirection.html> 

 ``` c
  case '>':
  case '<':
    rcmd = (struct redircmd*)cmd;
    close(rcmd->fd);
    if (open(rcmd->file, rcmd->mode, 0644) < 0) {
      fprintf(stderr, "open() %s error\n", rcmd->file);
      exit(-1);
    }
    runcmd(rcmd->cmd);
    break
 ```

## Implement pipes

http://man7.org/linux/man-pages/man2/pipe.2.html
http://man7.org/linux/man-pages/man2/dup.2.html

https://stackoverflow.com/questions/51004470/closing-unused-pipe-file-descriptors

Read end will see EOF after write end close the write fd.

``` c
  case '|':
    pcmd = (struct pipecmd*)cmd;
    pipe(p);
    if(fork1() == 0) {
      close(1);
      dup2(p[1], 1);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->left);
    }
    if(fork1() == 0) {
      close(0);
      dup2(p[0], 0);
      close(p[0]);
      close(p[1]);
      runcmd(pcmd->right);
    }
    close(p[0]);
    close(p[1]);
    wait(&r);
    wait(&r);
    break;
  }
```