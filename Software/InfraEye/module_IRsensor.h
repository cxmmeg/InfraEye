

#define TEMP_OFFSET_D           (50)
#define TEMP_SCALE_FACTOR_D     (128)

void IRsensor_Init(void);
uint8_t IRsensor_DataIsReady(void);
void IRsensor_UpdateMinMax(float* min_t, float* max_t, float* pixelValue);
void IRsensor_UpdateMinMax_u16(uint16_t* u16MinTemp, uint16_t* u16MaxTemp, uint16_t* pu16PixelValue);
uint8_t IRsensor_CurrentSubPage(void);
void IRsensor_LoadSubPage(float* pixelValue);
void IRsensor_LoadSubPage_u16(uint16_t* pixelValue);

