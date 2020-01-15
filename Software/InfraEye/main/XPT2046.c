#include <stdint.h>
#include "XPT2046.h"

void XPT_2046_Init()
{
    XPT2046_CS_DIR = 0;
    XPT2046_IRQ_DIR = 1;
    XPT2046_CS = 1;
    sendGetXPT2046(0x90); // Must send first byte for initialisation (IRQ is most important)
}


unsigned int sendGetXPT2046 (uint8_t byte)
{
    unsigned char SPICount;
    unsigned char SPIData;
    unsigned int  SPIDataIn;
    
    XPT2046_CS = 1;
    SCK = 0;
    SPIData = byte;
    
    XPT2046_CS = 0;
    for(SPICount = 0;SPICount < 8;SPICount++){
        if(SPIData & 0x80) // 0x80 = 0b10000000
        {
            SDO = 1;
        }
        else
        {
            SDO = 0;
        }

        SCK = 1;

        SCK = 0;
        SPIData <<=1; // Shift left for 1 bit
    }
    
    delay_ms(2); // Conversion delay

    SDO = 0;    
    
    SPIDataIn = 0;
    
    for(SPICount = 0; SPICount < 16; SPICount++)
    {
        SPIDataIn <<=1;
        SCK = 1;
        SPIDataIn += SDI;
        SCK = 0;
    }
    XPT2046_CS = 1;
    SPIDataIn >>=4;
    return (unsigned int) SPIDataIn;
}


unsigned int XPT_2046_GET_X()
{
    unsigned int x;
    x = sendGetXPT2046(0x90);
    x = ((x-Xmin)*LCD_WIDTH)/(Xmax-Xmin);
    if(x<=0) x = 0;
    if(x>=LCD_WIDTH) x = LCD_WIDTH;
    return (unsigned int) x;
    
}

unsigned int XPT_2046_GET_Y()
{
    unsigned int y;
    y = sendGetXPT2046(0xD0);
    y = ((y-Ymin)*LCD_HEIGHT)/(Ymax-Ymin);
    if(y<=0) y = 0;
    if(y>=LCD_HEIGHT) y = LCD_HEIGHT;
    return (unsigned int) y;
}
