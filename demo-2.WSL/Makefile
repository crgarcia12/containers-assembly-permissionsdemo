obj-m += lkm_example.o
all:
	make -C /home/crgarcia/WSL2-Linux-Kernel M=$(PWD) modules
clean:
	make -C /home/crgarcia/WSL2-Linux-Kernel M=$(PWD) clean
test:
	# We put a — in front of the rmmod command to tell make to ignore
	# an error in case the module isn’t loaded.
	-sudo rmmod lkm_example
	# Clear the kernel log without echo
	sudo dmesg -C
	# Insert the module
	sudo insmod lkm_example.ko
	# Display the kernel log
	dmesg
load:
	sudo mknod /dev/lkm_example c 241 0
unload:
	sudo rm /dev/lkm_example
	sudo rmmod lkm_example
full:
	make clean
	make
	make test
	make load