rm *.o -f
rm assem-main  -f
rm assem64-main  -f

if [ $1 = "-64" ];
then
    nasm -f elf64 -o assem64-main.o assem64-main.asm
    ld assem64-main.o -o assem64-main
    ./assem64-main
    return
fi

nasm -f elf64 -o assem-main.o assem-main.asm
ld assem-main.o -o assem-main
./assem-main

