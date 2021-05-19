#pragma once
#include <stdint.h>

namespace string_tools
{
    int atoi(char* str, int base=10);
    char* utoa(unsigned long long value, char *result, int base=10);
    char* itoa(int value, char* result, int base=10);
};