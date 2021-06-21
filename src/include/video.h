#pragma once
#include <stdint.h>

namespace video
{
    typedef uint32_t color_t;
    struct v2i
    {
        uint64_t x,y;
        v2i(uint64_t x=0,uint64_t y=0);
        v2i operator +(v2i b);
        v2i operator -(v2i b);
        v2i operator *(v2i b);
        v2i operator /(v2i b);

        v2i operator *(uint64_t l);
        v2i operator /(uint64_t l);
    };

    v2i get_screen_size();

    void init(color_t* framebuffer_address,uint64_t width, uint64_t height);

    extern color_t* framebuffer;
    extern uint64_t width, height;

    inline color_t &pixel(v2i position)
    {
        return framebuffer[position.x % width + (position.y % height) * width];
    }

    void draw_image(color_t* image, v2i size, v2i position);
    void clear(color_t color);

    color_t rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    color_t rgb(uint8_t r, uint8_t g, uint8_t b);
};