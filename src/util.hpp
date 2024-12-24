#pragma once

#include <iostream>

inline float rand_f(float min, float max)
{
    auto precision = 1000.f;
    return ((float)(std::rand() % (int)((max - min) * precision)) / precision) + min;
}

inline int min(int a, int b)
{
    return a > b ? b : a;
}
inline int max(int a, int b)
{
    return a > b ? a : b;
}
