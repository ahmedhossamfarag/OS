#include <stdint.h>


template <typename T>
T math_abs(T x)
{
    return x < 0 ? -x : x;
}

template <typename T>
T math_max(T x, T y)
{
    return x >= y ? x : y;
}

template <typename T>
T math_min(T x, T y)
{
    return x <= y ? x : y;
}