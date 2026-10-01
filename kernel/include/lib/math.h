#include <stdint.h>

int math_ciel(float x);

/* Return x as ciel multiple of m */
uint32_t math_cielm(uint32_t x, uint32_t m);

uint64_t math_cielm64(uint64_t x, uint64_t m);

/* Return x as floor multiple of m */
uint32_t math_floorm(uint32_t x, uint32_t m);

int math_abs(int x);


int math_max(int x, int y);

int math_min(int x, int y);