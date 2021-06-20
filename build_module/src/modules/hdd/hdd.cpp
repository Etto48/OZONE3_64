#include "include/hdd.h"

const uint16_t iBR = 0x01F0;
const uint16_t iCNL = 0x01F4;
const uint16_t iCNH = 0x01F5;
const uint16_t iSNR = 0x01F3;
const uint16_t iHND = 0x01F6;
const uint16_t iSCR = 0x01F2;
const uint16_t iERR = 0x01F1;
const uint16_t iCMD = 0x01F7;
const uint16_t iSTS = 0x01F7;
const uint16_t iDCR = 0x03F6;


void enable_irq()
{
	io::outb(iDCR, 0x00);
}
void disable_irq()
{
    io::outb(iDCR, 0x02);
}


void set_lba(uint32_t lba)
{
	uint8_t 
        lba_0 = lba,
	    lba_1 = lba >> 8,
	    lba_2 = lba >> 16,
	    lba_3 = lba >> 24;

	io::outb(iSNR, lba_0);
	io::outb(iCNL, lba_1);
	io::outb(iCNH, lba_2);
	uint8_t hnd = (lba_3 & 0x0F) | 0xE0;
	io::outb(iHND, hnd);
}


int main()
{
    
    while (true)
    {
    }
}