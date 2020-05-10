rm *.o
rm assem-main
nasm -f elf64 -o assem-main.o assem-main.asm
ld assem-main.o -o assem-main
./assem-main