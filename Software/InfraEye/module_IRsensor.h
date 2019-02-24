

void IRsensor_Init(void);
uint8_t IRsensor_DataIsReady(void);
void IRsensor_UpdateMinMax(float* min_t, float* max_t, uint16_t* pixelValue);
uint8_t IRsensor_CurrentSubPage(void);
void IRsensor_LoadSubPage(float* pixelValue);
void IRsensor_LoadSubPage_u16(uint16_t* pixelValue);

