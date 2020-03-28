#include <stdint.h>
#include <stdio.h>

int main(void) {
    uint64_t rcs = 0;
    int ring;

    asm ("mov %%cs, %0" : "=r" (rcs));
    ring = (int) (rcs & 3);
    printf("Hello, world. This program is running on ring %d!\n", ring);
    return 0;
}