# Homework: xv6 lazy page allocation

## Part One: Eliminate allocation from sbrk()

Page fault occurs because shell calls `malloc` system call in `runcmd` function, which calls `sbrk` system call underground.

## Part Two: Lazy allocation

[https://github.com/WLBF/xv6-public/commit/ce445cf982c57aacc9bf5a16972d5de473c19de2](https://github.com/WLBF/xv6-public/commit/ce445cf982c57aacc9bf5a16972d5de473c19de2)

