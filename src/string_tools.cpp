#include "include/string_tools.h"

namespace string_tools
{
        char* itoa(int value, char* result, int base) {
        // check that the base if valid
        if (base < 2 || base > 36) { *result = '\0'; return result; }

        char* ptr = result, *ptr1 = result, tmp_char;
        int tmp_value;

        do {
            tmp_value = value;
            value /= base;
            *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
        } while ( value );

        // Apply negative sign
        if (tmp_value < 0) *ptr++ = '-';
        *ptr-- = '\0';
        while(ptr1 < ptr) {
            tmp_char = *ptr;
            *ptr--= *ptr1;
            *ptr1++ = tmp_char;
        }
        return result;
    }

    char* utoa(unsigned long long value, char *result, int base)
    {
        // check that the base if valid
        if (base < 2 || base > 36) { *result = '\0'; return result; }

        char* ptr = result, *ptr1 = result, tmp_char;
        unsigned long long int tmp_value;

        do {
            tmp_value = value;
            value /= base;
            *ptr++ = "0123456789abcdefghijklmnopqrstuvwxyz" [tmp_value % base];
        } while ( value );

        *ptr-- = '\0';
        while(ptr1 < ptr) {
            tmp_char = *ptr;
            *ptr--= *ptr1;
            *ptr1++ = tmp_char;
        }
        return result;
    }

    int atoi(char* str, int base)
    {
        int res = 0; // Initialize result
        uint8_t begin=0;
        if(str[0]=='-'||str[0]=='+')
            begin=1;
        // Iterate through all characters of input string and update result
        for (int i = begin; str[i] != '\0'; ++i)
            res = res*base + str[i] - '0';
        if(str[0]=='-')
            res=-res;
        // return result.
        return res;
    }


};