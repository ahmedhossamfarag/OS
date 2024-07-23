#include "math.h"


int math_ciel(float x){
    int _x = (int) x;
    if(x > _x)
        return _x + 1;
    return _x;
}

uint32_t math_cielm(uint32_t x, uint32_t m)
{
    return x + (m - x%m)%m;
}

uint32_t math_floorm(uint32_t x, uint32_t m)
{
    return x - x%m;
}
