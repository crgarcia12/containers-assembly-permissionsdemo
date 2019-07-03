To compile in Linux
# this will produce hello.o ELF object file
nasm -f elf64 main.asm -o main.o
# this will produce hello executable
ld -s -o main main.o
