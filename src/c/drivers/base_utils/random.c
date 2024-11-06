#include "random.h"
#include "../vga/vga.h"

unsigned long seed = 12345;
const unsigned long a = 1664525;
const unsigned long c = 1013904223;
const unsigned long m = 4294967295;

unsigned long next_rand() {
    seed = (a * seed + c) % m;
    return seed;
}

void change_seed(int mul) {
    seed *= (next_rand() * mul) % m;
}

int custom_rand(unsigned int max) {
    return next_rand() % (max - 0  + 1) + 0;
}
