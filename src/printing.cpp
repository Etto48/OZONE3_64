#include "include/printing.h"

void put_char(char c, uint8_t color, uint8_t x, uint8_t y)
{
    ((char*)0xb8000)[(x%80+y*80)*2] = c;
    ((char*)0xb8000)[(x%80+y*80)*2+1] = color;
}
void kscroll()
{
    for(uint32_t i=0;i<80*24*2;i++)
    {
        ((char*)0xb8000)[i] = ((char*)0xb8000)[i+80*2];
    }
    for(uint32_t i=80*24*2;i<80*25*2;i++)
    {
        ((char*)0xb8000)[i]=0;
    }
}
void kprint(const char* string, uint8_t color)
{
    static uint8_t curx=0,cury=0;
    while(*string!=0)
    {
        if(*string=='\n')
        {
            do
            {
                put_char(' ',color,curx++,cury);
            } while (curx<80);
            curx=0;
            cury++;
            string++;
        }
        else
        {
            put_char(*string++,color,curx++,cury);
            cury+=curx/80;
            curx%=80;
        }
        if(cury==25)
        {
            cury--;
            kscroll();
        }
    }
}