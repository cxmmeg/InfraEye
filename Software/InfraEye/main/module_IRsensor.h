#include <stdint.h>
#include "MLX90640_API.h"

#define TEMP_OFFSET_D           (USER_TEMP_OFFSET)
#define TEMP_SCALE_FACTOR_D     (USER_TEMP_GAIN)
#define TEMP_SCALE_BIT_SHIFT_D	(USER_TEMP_GAIN_BIT_SHIFT)

uint8_t IRsensor_Init(void);
uint8_t IRsensor_u8DataIsReady(void);
void IRsensor_UpdateMinMax(float* min_t, float* max_t, float* pixelValue);
void IRsensor_UpdateMinMax_u16(uint16_t* u16MinTemp, uint16_t* u16MaxTemp, uint16_t* pu16PixelValue);
uint8_t IRsensor_u8CurrentSubPage(void);
void IRsensor_LoadSubPage(float* pixelValue);
void IRsensor_vLoadSubPage_u16(void);
void IRsensor_vCalculatePixelTemp_u16(uint16_t* pu16PixelValue);

