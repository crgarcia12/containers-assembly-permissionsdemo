# Description

This is a clone of Demo-2, but running inside WSL (Ubuntu 20)
In here we will create a kernel module to run priviledged instructions on WSL

# Initial setup
Following instructions from:
https://blog.sourcerer.io/writing-a-simple-linux-kernel-module-d9dc3762c234

And
https://unix.stackexchange.com/questions/594470/wsl-2-does-not-have-lib-modules/645131?noredirect=1#comment1211111_645131

Other good things are:

Taking instructions from Linux device drivers book
https://learning.oreilly.com/library/view/linux-device-drivers/9781785280009/77b7fd36-fa66-45c1-9b38-65baf42c0d34.xhtml

# Running the example
To run this example, you need your own WSL Kernel, since the default one does not have headers

## Prepare the environment
```
cd ~
sudo git clone https://github.com/microsoft/WSL2-Linux-Kernel.git

sudo apt install build-essential flex bison libssl-dev libelf-dev

cd WSL2-Linux-Kernel

cp Microsoft/config-wsl .config (if you've compiled previously, this may require sudo)

sudo make -j $(expr $(nproc) - 1)
```
From Windows, copy \\wsl$\Ubuntu\home\WSL_USERNAME\WSL2-Linux-Kernel\arch\x86\boot\bzimage to C:\Users\crgar

If you cannot find it, use in wsl
cd ~
find . -name "bzimage"

Create a file called `.wslconfig` in `C:\Users\crgar`
Note: The double slashes (\\) are intentional. Also it seems there used to be a bug related to trailing white space. Feel free to test and possibly troubleshoot this or you can just remove any trailing white space to be safe.
```
[wsl2]
kernel=C:\\Users\\crgar\\bzimage
```

From PowerShell, run `wsl --shutdown`
make sure nothing is running with `wsl --list --running`
Reopen your flavor of WSL2.

## Build the driver
Take a look at the make file, make sure the paths are correct
```
sudo bash
make
```

## Load the driver
```
make test
```

## invoke the driver

### Check the major number from the output of the previous command y use it to create a device file
```
sudo mknod /dev/lkm_example c <MAJOR> 0
```

### Test it
```
cat /dev/lkm_example
```

### To get kernel logs
```
dmesg
```

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
