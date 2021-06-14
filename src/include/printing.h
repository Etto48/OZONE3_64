#pragma once
#include <stdint.h>
#include <stdarg.h>
#include "memory.h"
#include "string_tools.h"
#include "math.h"

void set_fb(void *fb_loc, uint64_t _fb_width, uint64_t _fb_height, uint8_t _fb_depth);
void put_char(char c, uint8_t color, uint8_t x, uint8_t y);
void clear(uint8_t color);
void kscroll();
void kprint(const char* string, uint8_t color);
void println(const char* str);
int printf(const char* fmt, ...);
int vsnprintf(char* str, size_t size, const char* fmt, va_list ap);
int snprintf(char* buf, uint32_t n, const char* fmt, ...);