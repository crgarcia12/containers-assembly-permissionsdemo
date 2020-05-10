# Clean the project
. clean.sh

# Demo 1: Build Assembly
Run this 
. assem-build.sh

Or run this for 64 bits (WSL)
. assem-build.sh -64 

To compile in Linux
### this will produce main.o ELF object file
nasm -f elf64 assem-main.asm -o assem-main.o
### this will produce main executable
ld -s -o assem-main assem-main.o

# Demo 2: build C
To build the C++ run this

```
. c-build.sh
gcc c-main.c -o c-main
./c-main

# Decompile:

objdump -d c-main > c-main.asm
```

# Demo 3: Set cgroups manually

```
# These are the different controllers which are created by the kernel itself.
ls -l /sys/fs/cgroup/

# Each of these controllers have their own tunables for example
ls -l /sys/fs/cgroup/cpuacct/
```

## Demo usernamespace:
```
# create a new namespace and run bash on it
sudo unshare -u bash

# Check the machine name (inside and outside namespace)
uname -n

# change machine name inside namespace
hostname CarlosContainer

# run unname again in both

# What namespae am I in?
readlink /proc/$$/ns/uts

```

# get a terminal in docker
docker run -v /tmp:/mnt3 -it ubuntu /bin/sh

## Other linux commands
nsenter --target --mount : run program with namespaces of other programm

## Jump into a container file system
```
docker run -it -v /tmp3:/mnt3 ubuntu /bin/sh
watch ls 
```

In another bash, you can enter to that mount namespace like this:
1. Find the Pid
ps -ahl | grep watch
4     0 21466 19843  20   0  11572  2620 -      S+   pts/0      0:00 watch ls

sudo nsenter --target 21466 --mount
cd mnt3