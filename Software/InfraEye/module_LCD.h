

void LCD_Init(void);
void LCD_PrintStats(uint16_t x, uint16_t y, float max_t, float min_t, float FPS);
void LCD_drawImage(uint16_t *colorArray, uint16_t columns, uint16_t rows, uint16_t x0, uint16_t y0, uint8_t pixelSize, uint8_t debug);
void LCD_Convert(float *frameTemperature, uint16_t *frameColor, uint16_t frameSize,
  float minTemperature, float maxTemperature, uint8_t subPage);
void LCD_Convert_u16(uint16_t *frameTemperature, uint16_t *frameColor, uint16_t frameSize,
  float minTemperature, float maxTemperature, uint8_t subPage);
void LCD_FillRect(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);


