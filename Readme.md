# ASEM
Run this . build.sh

To compile in Linux
# this will produce main.o ELF object file
nasm -f elf64 main.asm -o main.o
# this will produce main executable
ld -s -o main main.o

# C++
. hello.sh

gcc hello.c -o hello
./hello
objdump -d hello > hello.asm