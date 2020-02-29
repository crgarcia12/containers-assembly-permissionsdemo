rm *.o
rm main
nasm -f elf64 -o main.o main.asm
ld main.o -o main
./main