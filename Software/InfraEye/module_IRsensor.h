

void IRsensor_Init(void);
uint8_t IRsensor_DataIsReady(void);
void IRsensor_UpdateMinMax(float* min_t, float* max_t, float* pixelValue);
uint8_t IRsensor_CurrentSubPage(void);
void IRsensor_LoadSubPage(float* pixelValue);

