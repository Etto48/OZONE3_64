#pragma once

#include <stdint.h>
#include <stddef.h>

#define max(A,B) ((A)>(B))?(A):(B)
#define min(A,B) ((A)<(B))?(A):(B)
#define abs(A) max((A),-(A))

typedef struct _point
{
    _point(uint64_t _x,uint64_t _y):x(_x),y(_y){}
    uint64_t x,y;
}point;

double pow(double x, int64_t n);