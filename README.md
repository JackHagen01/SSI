# P1: Simple Shell Interpreter (SSI)

## Overview

A basic command-line shell written in C, executes user commands from input. Created for CSC360: Operating Systems.

## Features

- Prompt showing username, hostname, and current working directory
- Execution of any command existing on Linux system
- Input is parsed with strtok() and executed with fork() and execvp()
- Support for changing directories via `cd` command
- Run background processes with `bg`
- Background jobs are kept in a singly linked list
- List current background jobs with `bglist`
- Current foreground process ended with CTRL-C
- Exits on CTRL-D

## Compilation and Execution

Compile with: 
`gcc ssi.c -o ssi` 
or using provided Makefile:
`make`

Run with: 
`./ssi`

## Use Examples

### Basic Linux Shell Commands
```
username@hostname: /home/user/subdir > ls
ssi.c Makefile README.md
username@hostname: /home/user/subdir > mkdir temp
username@hostname: /home/user/subdir > ls
ssi.c Makefile README.md temp
username@hostname: /home/user/subdir > rm -rf temp
username@hostname: /home/user/subdir > ls
ssi.c Makefile README.md
username@hostname: /home/user/subdir > cat Makefile
ssi: ssi.c
        gcc ssi.c -o ssi
username@hostname: /home/user/subdir > 
```

### Changing Directories
```
username@hostname: /home/user/subdir > cd ..
username@hostname: /home/user > cd subdir
username@hostname: /home/user/subdir > cd ~
username@hostname: /home/user >
```

### Background Execution
Note that the shell prompt may not show correctly when the background process gives output.
User input is prefixed with `>` for clarity in the example.
```
username@hostname: /home/user/subdir > bg ping -c 5 1.1.1.1
PING 1.1.1.1 (1.1.1.1) 56(84) bytes of data.
64 bytes from 1.1.1.1: icmp_seq=1 ttl=54 time=3.15 ms
64 bytes from 1.1.1.1: icmp_seq=2 ttl=54 time=2.96 ms
> bglist
277458: ping -c 5 1.1.1.1
Total Background Jobs: 1
64 bytes from 1.1.1.1: icmp_seq=3 ttl=54 time=2.96 ms
64 bytes from 1.1.1.1: icmp_seq=4 ttl=54 time=3.03 ms
64 bytes from 1.1.1.1: icmp_seq=5 ttl=54 time=3.07 ms

--- 1.1.1.1 ping statistics ---
5 packets transmitted, 5 received, 0% packet loss, time 4001ms
rtt min/avg/max/mdev = 2.960/3.033/3.148/0.070 ms
> bglist
277458: ping -c 5 1.1.1.1  has terminated.
Total Background Jobs: 0
username@hostname: /home/user/subdir > 
```

### Signal Handling
```
username@hostname: /home/user/subdir > ping 1.1.1.1
PING 1.1.1.1 (1.1.1.1) 56(84) bytes of data.
64 bytes from 1.1.1.1: icmp_seq=1 ttl=54 time=2.91 ms
64 bytes from 1.1.1.1: icmp_seq=2 ttl=54 time=3.11 ms
64 bytes from 1.1.1.1: icmp_seq=3 ttl=54 time=2.97 ms
^C
--- 1.1.1.1 ping statistics ---
3 packets transmitted, 3 received, 0% packet loss, time 2003ms
rtt min/avg/max/mdev = 2.912/2.996/3.112/0.084 ms
username@hostname: /home/user/subdir >
```

## Author

Jackson Hagen V01029307
