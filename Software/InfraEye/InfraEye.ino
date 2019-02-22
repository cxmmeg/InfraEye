#include "module_ImageUpscaling.h"
#include "module_LCD.h"
#include "module_IRsensor.h"
#include "config.h"

#ifdef LIVE_DATA
  static float pixelValue[768];
  #endif

void setup()
{
  // Initialize serial port for debugging
  Serial.begin(115200);
  // Initialize LCD
  LCD_Init();
  IRsensor_Init();   
  Serial.println("\nInfraEye boot...");  
}

void loop(void)
{
  uint16_t data;
  static float afUpscaleBuffer[OUTPUT_BUFFER_SIZE_D]; 
  int status;
  static uint16_t frameColor[OUTPUT_BUFFER_SIZE_D]; // scaled
  static uint16_t frameColor1[768];                 // not scaled
  static float min_t = 300;
  static float max_t = -40;
  static float FPS = 0;
  static unsigned long time_start, time_1, time_2, time_3, time_4, time_5, time_end;
  uint8_t subPage;
  uint8_t dataReady = 0;
  uint8_t error;
  static uint16_t u16DisplayIterator = 0;
  uint16_t u16DispCoordinateX, u16DispCoordinateY;

  // -------------- Wait for new subpage- --------------------
//  time_start = micros();
  while(dataReady == 0)
  {
    dataReady = IRsensor_DataIsReady();
    wdt_reset();
  }
  subPage = IRsensor_CurrentSubPage();  
  Serial.printf("Subpage %d\n", subPage);
  //if(error != 0)
  {
    //Serial.printf("Error while reading status register. Error code:%d\n", error);      
  }    
  time_1 = micros();

  // -------------- Read subframe ----------------------------
  IRsensor_LoadSubPage(pixelValue);
  
  //Serial.printf("Status:%d\n", status);
  time_2 = micros();  
    
  time_3 = micros();
  if((micros()/1000000)%2 == 0)
  {
    IRsensor_UpdateMinMax(&min_t, &max_t, pixelValue);    
  }
  // -------------- Upscale ----------------------------------
  if(subPage==0)
  {
    time_end = micros();
    FPS = (float)1000000 / ((float)time_end-(float)time_start);
    time_start = micros();    
  }
  if(subPage==1)  // All pixels are gathered
  {
    
  }
  for (uint16_t u16Iterator = 0; u16Iterator < (OUTPUT_NUM_OF_PIXELS_D / OUTPUT_BUFFER_SIZE_D); u16Iterator++)
  {
    
    // -------------- Upscale temperature image ---------------
    img_up_vUpscaleImage(pixelValue, afUpscaleBuffer, OUTPUT_BUFFER_SIZE_D);
  
    // -------------- Convert temperature to color code -------

    LCD_Convert(afUpscaleBuffer, frameColor, OUTPUT_BUFFER_SIZE_D, min_t, max_t, subPage);
        
    for(uint16_t i = 0; i < OUTPUT_BUFFER_SIZE_D; i++)
    {
      u16DispCoordinateX = OUTPUT_ARRAY_WIDTH_D - (u16DisplayIterator / OUTPUT_ARRAY_LENGTH_D);        
      u16DispCoordinateY = u16DisplayIterator % OUTPUT_ARRAY_LENGTH_D;        
      /* Scaling factor is 4 - display pixel as 2x2 square */
      LCD_FillRect(2 * u16DispCoordinateX, 2* u16DispCoordinateY, 2, 2, frameColor[i]);      
      //tft.drawPixel(u16DispCoordinateY, u16DispCoordinateX, frameColor[i]);               
      u16DisplayIterator++;        
    }
    if(IRsensor_DataIsReady()) break;  // We need to get new data -> Image processing interrupted
  }
  if(IRsensor_DataIsReady()==0)
  { // Whole image has been processed, reset upscaling
      img_up_vResetUpscaling();
      u16DisplayIterator = 0;
  }
  time_4 = micros();
  wdt_reset();

#if 0
  LCD_Convert(mlx90640To, frameColor1, 768, colorPalete, 403, min_t, max_t, subPage);
  #ifdef COLOR2CONSOLE
  printColorToConsole(frameColor1, 768, 32);
  #endif
  LCD_drawImage(frameColor1, 32, 24, 120, 0, 4, 0);
#endif

  if((micros()/1000000)%2 == 0) LCD_PrintStats(0, 190, max_t, min_t, FPS);
  time_5 = micros();
  
  Serial.printf("Waiting for data ready%d us\tGetFrame %dus\tCalculateTemperature %dus\nUpscale, Convert, write do LCD %dus\tDisplay text %dus\tTotal period %dms\n", time_1-time_start, time_2-time_1, time_3-time_2, time_4-time_3, time_5-time_4, time_end-time_5, (time_end-time_start)/1000);
}

void printTemperaturesToConsole(float *mlx90640To, uint16_t pixelCount, uint8_t rowLength)
{
  for(uint16_t i=0;i<768;i++)
  {
    if((i%32)==0) Serial.printf("\n");
    Serial.printf("%f, ", mlx90640To[i]);
  }
  Serial.printf("\n");
}

void printColorToConsole(uint16_t *color, uint16_t pixelCount, uint8_t rowLength)
{
  for(uint16_t i=0;i<768;i++)
  {
    if((i%32)==0) Serial.printf("\n");
    Serial.printf("%x ", color[i]);
  }
  Serial.printf("\n");
}
