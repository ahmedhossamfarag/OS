#include "math.h"


int math_ciel(float x){
    int _x = (int) x;
    if(x > _x)
        return _x + 1;
    return _x;
}