#include "include/video.h"

namespace video
{
    typedef uint32_t color_t;

    v2i::v2i(uint64_t x, uint64_t y) : x(x), y(y) {}
    v2i v2i::operator+(v2i b)
    {
        return {x + b.x, y + b.y};
    }
    v2i v2i::operator-(v2i b)
    {
        return {x - b.x, y - b.y};
    }
    v2i v2i::operator*(v2i b)
    {
        return {x * b.x, y * b.y};
    }
    v2i v2i::operator/(v2i b)
    {
        return {x / b.x, y / b.y};
    }

    v2i v2i::operator*(uint64_t l)
    {
        return {x * l, y * l};
    }
    v2i v2i::operator/(uint64_t l)
    {
        return {x / l, y / l};
    }

    color_t *framebuffer = nullptr;
    uint64_t width = 0, height = 0;

    v2i get_screen_size()
    {
        return {width, height};
    }

    void init(color_t *framebuffer_address, uint64_t fb_width, uint64_t fb_height)
    {
        framebuffer = framebuffer_address;
        width = fb_width;
        height = fb_height;
        clear(rgb(0, 0, 0));
    }

    void draw_image(color_t *image, v2i size, v2i position)
    {
        for (uint64_t y = 0; y < size.y; y++)
        {
            for (uint64_t x = 0; x < size.x; x++)
            {
                pixel({x + position.x, y + position.y}) = image[x + y * size.x];
            }
        }
    }

    void clear(color_t color)
    {
        for (uint64_t y = 0; y < height; y++)
        {
            for (uint64_t x = 0; x < width; x++)
            {
                pixel({x, y}) = color;
            }
        }
    }

    color_t rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {
        //0xbbggrraa
        return a << 24 | r << 16 | g << 8 | b;
    }
    color_t rgb(uint8_t r, uint8_t g, uint8_t b)
    {
        return rgba(r, g, b, 0xff);
    }
};