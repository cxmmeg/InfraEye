#include <stdint.h>

#define XPT2046_CS LATDbits.LATD5 // XPT2046 Chip select
#define XPT2046_IRQ PORTDbits.RD11 // XPT2046 IRQ 

#define XPT2046_CS_DIR TRISDbits.TRISD5 // XPT2046 Chip select Direction
#define XPT2046_IRQ_DIR TRISDbits.TRISD11 // XPT2046 IRQ Direction



unsigned int sendGetXPT2046 (uint8_t byte); // SPI send to XPT2046
unsigned int XPT_2046_GET_X(); // Get X coordinate
unsigned int XPT_2046_GET_Y(); // Get Y coordinate
void XPT_2046_Init(); // Initialise XPT2046
