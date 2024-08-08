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

uint32_t math_abs(uint32_t x)
{
    return x < 0 ? -x : x;
}

uint32_t math_max(uint32_t x, uint32_t y){
    return x >= y ? x : y;
}

uint32_t math_min(uint32_t x, uint32_t y){
    return x <= y ? x : y;
}
