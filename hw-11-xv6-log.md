# Homework: xv6 log

[https://github.com/WLBF/xv6-public/commit/17d90f9e01808a07ae978ad44d35a158bdf781be](https://github.com/WLBF/xv6-public/commit/17d90f9e01808a07ae978ad44d35a158bdf781be)

Make sure you understand what happened. Which of the file creation's modifications were written to the disk before the crash, and which were not?

The inode modification is write to disk and the file content was wirite to incorrect block 0.



Why was the file empty, even though you created it with `echo hi > a`?

```sh
recovery: n=2
init: starting sh    
$ cat a
$
```

After reboot, the file content to write is no longer in the buffer cache, so after recovery the newly create file `a` is empty.   



Make sure you understand why it would be a mistake for the buffer cache to evict block 33 from the buffer cache before the commit.

The block must stay in the cache until committed: until then, the cached copy is the only record of the modification.

