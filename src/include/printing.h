#pragma once
#include <stdint.h>
#include "memory.h"

void put_char(char c, uint8_t color, uint8_t x, uint8_t y);
void clear(uint8_t color);
void kscroll();
void kprint(const char* string, uint8_t color);