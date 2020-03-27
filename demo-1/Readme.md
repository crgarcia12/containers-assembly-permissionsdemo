# ASEM
Run this 
. build.sh

To compile in Linux
### this will produce main.o ELF object file
nasm -f elf64 main.asm -o main.o
### this will produce main executable
ld -s -o main main.o

# C++
To build the C++ run this

```
. hello.sh
gcc hello.c -o hello
./hello

# Decompile:

objdump -d hello > hello.asm
```

# To set cgroups manually

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
nsenter: run program with namespaces of other programm