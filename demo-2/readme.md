## Description

In here we will create a kernel module to run priviledge instructions

## Initial setup
Following instructions from:
https://blog.sourcerer.io/writing-a-simple-linux-kernel-module-d9dc3762c234

Other good things are:

Taking instructions from Linux device drivers book
https://learning.oreilly.com/library/view/linux-device-drivers/9781785280009/77b7fd36-fa66-45c1-9b38-65baf42c0d34.xhtml

## Running the example

sudo bash
make

# load the module and check that it runned
insmod lkm_example.ko
dmesg | grep "Hello"

# is it still running?
lsmod | grep "lkm_example"

# removing the module
rmmod lkm_example

# another easy way
make test