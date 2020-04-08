# Description

In here we will create a kernel module to run priviledged instructions

# Initial setup
Following instructions from:
https://blog.sourcerer.io/writing-a-simple-linux-kernel-module-d9dc3762c234

Other good things are:

Taking instructions from Linux device drivers book
https://learning.oreilly.com/library/view/linux-device-drivers/9781785280009/77b7fd36-fa66-45c1-9b38-65baf42c0d34.xhtml

# Running the example
We have created a Makefile with the commands needed to build and load the module. 
To run it, get a root console:

sudo bash

## Build the driver
make

## Load the driver
make test

## invoke the driver

### Check the major number from the output of the previous command y use it to create a device file
sudo mknod /dev/lkm_example c <MAJOR> 0

### Test it
cat /dev/lkm_example

### To get kernel logs
dmesg

# The driver will make you jump your namespace:
1. look at my mount namespace
ls -Li /proc/$$/ns/mnt

2. unshare
unshare --mount
bash
ls -Li /proc/$$/ns/mnt

3. run the driver and check your new namespace
cat /dev/lkm_example
ls -Li /proc/$$/ns/mnt