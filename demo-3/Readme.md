# Get a terminal in docker
docker run -v /tmp3:/mnt3 -it ubuntu /bin/sh

# Write a file from the container
## in docker
cd /mnt3
touch newfile.txt

## in both
cd /tmp3
ls -lah

## in the container
watch ps ax

## outside 
ps ax
find "watch ps ax"
pstree -p > tree.txt
serach for watch, dockerd

# How is a container created?
sudo strace -f -p `pidof containerd` -o strafce_log > syscalls.txt