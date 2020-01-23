#include <stdint.h>

#define PIN_NUM_CS	5

uint32_t XPT_2046_Init(void); // Initialise XPT2046
unsigned int XPT_2046_GET_X(void); // Get X coordinate
unsigned int XPT_2046_GET_Y(void); // Get Y coordinate
