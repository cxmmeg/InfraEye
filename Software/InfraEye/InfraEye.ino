#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "MLX90640_API.h"
#include "MLX90640_I2C_driver.h"
#include <Wire.h>
#include "module_ImageUpscaling.h"

//#define STMPE_CS 16 // Not used yet
#define TFT_CS   0
#define TFT_DC   2//4//15
// #define SD_CS    0 // Not used yet
#define RAW_IMAGE

//#define TEMPERATURE2CONSOLE   // Debug print of 768 temperatures to console
//#define COLOR2CONSOLE   // Debug print of 768 uint16_t colors to console

#define MLX90640_ADR 0x33
#define TA_SHIFT 8 //the default shift for a MLX90640 device in open air
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

uint16_t eeMLX90640[832];
paramsMLX90640 mlx90640;
const uint16_t colorPalete[] = {0x1,0x2,0x3,0x4,0x5,0x5,0x6,0x6,0x7,0x7,0x8,0x8,0x9,0x9,0xa,0xa,0xa,0xb,0xb,0xb,0xc,0xc,0xc,0xc,0xd,0x80d,0x80d,0x80e,0x80e,0x80e,0x80e,0x80e,0x80e,0x100f,0x100f,0x100f,0x100f,0x100f,0x180f,0x1810,0x1810,0x1810,0x2010,0x2010,0x2010,0x2010,0x2811,0x2811,0x2811,0x2811,0x3011,0x3011,0x3011,0x3011,0x3811,0x3812,0x3812,0x3812,0x3812,0x3812,0x4012,0x4012,0x4012,0x4012,0x4012,0x4812,0x4812,0x4812,0x4812,0x4812,0x5012,0x5013,0x5013,0x5013,0x5813,0x5813,0x5813,0x5813,0x5813,0x6013,0x6013,0x6013,0x6013,0x6813,0x6813,0x6813,0x6813,0x6813,0x7013,0x7013,0x7013,0x7013,0x7013,0x7813,0x7813,0x7813,0x7813,0x7813,0x8013,0x8013,0x8013,0x8013,0x8013,0x8813,0x8813,0x8813,0x8813,0x8813,0x9013,0x9013,0x9013,0x9013,0x9813,0x9813,0x9813,0x9813,0x9813,0x9813,0xa013,0xa013,0xa013,0xa013,0xa013,0xa013,0xa813,0xa813,0xa813,0xa813,0xa813,0xa813,0xa813,0xb013,0xb013,0xb012,0xb012,0xb012,0xb012,0xb012,0xb012,0xb012,0xb812,0xb832,0xb832,0xb832,0xb832,0xb832,0xb832,0xb832,0xb832,0xb832,0xc032,0xc032,0xc052,0xc052,0xc051,0xc051,0xc051,0xc071,0xc071,0xc071,0xc071,0xc071,0xc891,0xc890,0xc890,0xc890,0xc890,0xc8b0,0xc8b0,0xc8b0,0xc8b0,0xc8cf,0xc8cf,0xc8cf,0xd0cf,0xd0cf,0xd0ee,0xd0ee,0xd0ee,0xd0ee,0xd10e,0xd10d,0xd10d,0xd10d,0xd12d,0xd12c,0xd12c,0xd92c,0xd94c,0xd94c,0xd94b,0xd96b,0xd96b,0xd96a,0xd96a,0xd98a,0xd989,0xd989,0xd988,0xd9a8,0xd9a8,0xd9a7,0xe1a7,0xe1c6,0xe1c6,0xe1c6,0xe1c5,0xe1e5,0xe1e4,0xe1e4,0xe1e4,0xe203,0xe203,0xe203,0xe223,0xe223,0xe222,0xe222,0xe242,0xe242,0xea42,0xea62,0xea61,0xea61,0xea61,0xea61,0xea61,0xea81,0xea81,0xea81,0xea81,0xeaa1,0xeaa1,0xeaa1,0xeac1,0xeac0,0xeac0,0xeac0,0xeae0,0xeae0,0xeae0,0xeae0,0xeae0,0xeb00,0xeb00,0xeb00,0xf300,0xf320,0xf320,0xf320,0xf320,0xf320,0xf340,0xf340,0xf340,0xf340,0xf340,0xf360,0xf360,0xf360,0xf360,0xf380,0xf380,0xf380,0xf380,0xf3a0,0xf3a0,0xf3a0,0xf3a0,0xf3c0,0xf3c0,0xf3c0,0xf3e0,0xf3e0,0xf3e0,0xf400,0xf400,0xf400,0xf400,0xf420,0xf420,0xf420,0xfc20,0xfc40,0xfc40,0xfc40,0xfc40,0xfc40,0xfc60,0xfc60,0xfc60,0xfc60,0xfc60,0xfc80,0xfc80,0xfc80,0xfc80,0xfca0,0xfca0,0xfca0,0xfcc0,0xfcc0,0xfcc0,0xfce0,0xfce0,0xfce0,0xfd00,0xfd00,0xfd00,0xfd00,0xfd20,0xfd20,0xfd20,0xfd40,0xfd40,0xfd40,0xfd60,0xfd60,0xfd60,0xfd60,0xfd80,0xfd80,0xfd80,0xfd80,0xfda0,0xfda0,0xfda0,0xfdc0,0xfdc0,0xfdc0,0xfdc0,0xfdc0,0xfde0,0xfde0,0xfde0,0xfe00,0xfe00,0xfe00,0xfe00,0xfe20,0xfe20,0xfe20,0xfe20,0xfe40,0xfe40,0xfe40,0xfe40,0xfe40,0xfe60,0xfe60,0xfe60,0xfe60,0xfe60,0xfe80,0xfe80,0xfe81,0xfea1,0xfea1,0xfea1,0xfea1,0xfec1,0xfec1,0xfec1,0xfec1,0xfec2,0xfee2,0xfee2,0xfee2,0xfee3,0xfee3,0xfee3,0xff04,0xff04,0xff04,0xff04,0xff05,0xff25,0xff25,0xff26,0xff26,0xff27,0xff27,0xff47,0xff48,0xff48,0xff49,0xff49,0xff6a,0xff6a,0xff6a,0xff6b,0xff6b,0xff6c,0xff6c,0xff8d,0xff8d,0xff8e,0xff8e,0xff8f,0xff90,0xff90,0xff91,0xff91,0xffb2,0xffb2,0xffb3,0xffb3,0xffb4,0xffb4,0xffb5,0xffb5,0xffb6,0xffb6,0xffd7,0xffd7,0xffd8,0xffd8,0xffd8,0xffd9,0xffd9,0xffda,0xffda,0xffdb,0xfffb,0xfffb,0xfffc,0xfffc,0xfffd,0xfffd,0xfffd,0xfffe,0xfffe,0xfffe};

void setup()
{
  int result = -55;
  uint16_t data;
  uint8_t address;
  int status; 

  // Initialize serial port for debugging
  Serial.begin(115200);
  // Initialize LCD
  tft.begin();
  tft.fillRect(0, 0, 240, 320, 0x00031);
  // Initialize I2C
  Wire.begin();  
  Wire.setClock(800000);
  Serial.println("\nInfraEye boot...");
  // Scan I2C to make sure IR sensor is present
  address = I2C_scan();
  // Initialize IR sensor
  MLX90640_SetResolution(MLX90640_ADR, 0x03);
  Serial.println("Get MLX90640 resolution:");
  result = MLX90640_GetCurResolution(0x33);
  Serial.println(result);
  MLX90640_SetRefreshRate(MLX90640_ADR, 0x03);
  MLX90640_SetInterleavedMode(MLX90640_ADR);  
  status = MLX90640_DumpEE (MLX90640_ADR, eeMLX90640);
  status = MLX90640_ExtractParameters(eeMLX90640, &mlx90640); 
}

void loop(void)
{
  int result = -55;
  uint16_t data;
  char str_temp[6];
  float emissivity = 0.95;
  float tr;
  static uint16_t mlx90640Frame[834];
  static float mlx90640To[768];
  static float afUpscaleBuffer[OUTPUT_BUFFER_SIZE_D]; 
  int status;
  static uint16_t frameColor[OUTPUT_BUFFER_SIZE_D]; // scaled
  static uint16_t frameColor1[768];                 // not scaled
  float min_t = 300;
  float max_t = -40;
  static float FPS = 0;
  unsigned long time_start, time_1, time_2, time_3, time_4, time_5, time_end;
  uint8_t subPage;
  uint8_t dataReady = 0;
  uint16_t registerValue;
  uint8_t error;
    uint16_t u16DisplayIterator;
  uint16_t u16DispCoordinateX, u16DispCoordinateY;


  // -------------- Wait for new subpage- --------------------
  time_start = micros();
  while(dataReady == 0)
  {
    error = MLX90640_I2CRead(MLX90640_ADR, 0x8000, 1, &registerValue);
    if(error != 0)
    {
      Serial.printf("Error while reading status register. Error code:%d\n", error);      
    }    
    dataReady = registerValue & 0x0008;
    wdt_reset();
  }
  subPage = registerValue & 0x0001;
  Serial.printf("Subpage %d\n", subPage);
  //error = MLX90640_I2CWrite(MLX90640_ADR, 0x8000, (registerValue && ~0x0008));
  //if(error != 0)
  {
    //Serial.printf("Error while reading status register. Error code:%d\n", error);      
  }    
  time_1 = micros();

  // -------------- Read subframe ----------------------------
  status = MLX90640_GetFrameData(MLX90640_ADR, mlx90640Frame);
  //Serial.printf("Status:%d\n", status);
  time_2 = micros();

  // -------------- Calculate temperature of subframe --------
  tr = MLX90640_GetTa(mlx90640Frame, &mlx90640) - TA_SHIFT; //reflected temperature based on the sensor
  dtostrf((double)tr, 4, 2, str_temp);
  Serial.printf("Ambient temperature: %s°C (%d)\n", str_temp, (int)tr);
  wdt_reset();
  MLX90640_CalculateTo(mlx90640Frame, &mlx90640, emissivity, tr, mlx90640To);
  
  #ifdef TEMPERATURE2CONSOLE
  printTemperaturesToConsole(mlx90640To, 768, 32);
  #endif
  
  time_3 = micros();
  
  // -------------- Wait for RR - 20 % -----------------------
  for(uint16_t i=0;i<768;i++)
  {
    if(mlx90640To[i] > max_t) max_t = mlx90640To[i];
    if(mlx90640To[i] < min_t) min_t = mlx90640To[i];    
  }
  // -------------- Upscale ----------------------------------
  img_up_vResetUpscaling();
  u16DisplayIterator = 0;
  
  for (uint16_t u16Iterator = 0; u16Iterator < (OUTPUT_NUM_OF_PIXELS_D / OUTPUT_BUFFER_SIZE_D); u16Iterator++)
  {
    
    // -------------- Upscale temperature image ---------------
    img_up_vUpscaleImage(mlx90640To, afUpscaleBuffer, OUTPUT_BUFFER_SIZE_D);
  
    // -------------- Convert temperature to color code -------

    Convert(afUpscaleBuffer, frameColor, OUTPUT_BUFFER_SIZE_D, colorPalete, 403, min_t, max_t, subPage);
        
    for(uint16_t i = 0; i < OUTPUT_BUFFER_SIZE_D; i++)
    {
      u16DispCoordinateY = u16DisplayIterator / OUTPUT_ARRAY_LENGTH_D;
      //if((u16DispCoordinateY % 2) == subPage)
      {
        u16DispCoordinateX = OUTPUT_ARRAY_LENGTH_D - (u16DisplayIterator % OUTPUT_ARRAY_LENGTH_D);
        //u16DispCoordinateX = ((OUTPUT_ARRAY_LENGTH_D - 1) - (u16DisplayIterator % OUTPUT_ARRAY_LENGTH_D));
        /* Scaling factor is 4 - display pixel as 2x2 square */
        tft.fillRect(2 * u16DispCoordinateY, 2* u16DispCoordinateX, 2, 2, frameColor[i]);
        //tft.drawPixel(u16DispCoordinateY, u16DispCoordinateX, frameColor[i]);
      }
      
      u16DisplayIterator++;

      
    }
  }
  time_4 = micros();
  wdt_reset();

#if 0
  Convert(mlx90640To, frameColor1, 768, colorPalete, 403, min_t, max_t, subPage);
  #ifdef COLOR2CONSOLE
  printColorToConsole(frameColor1, 768, 32);
  #endif
  drawImage(frameColor1, 32, 24, 120, 0, 4, 0);
#endif
  
  tft.fillRect(0, 200, 240, 80, 0x0000);
  tft.setCursor(10, 200);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(2);
  dtostrf((double)max_t, 3, 0, str_temp);
  tft.printf("Tmax=%s°C", str_temp);
  tft.setCursor(10, 240);
  dtostrf((double)min_t, 3, 0, str_temp);
  tft.printf("Tmin=%s°C", str_temp);
  dtostrf((double)FPS, 2, 1, str_temp);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);
  tft.setCursor(10, 270);
  tft.printf("%sFPS", str_temp);
  time_5 = micros();
//  MLX90640_BadPixelsCorrection((&mlx90640)->brokenPixels, mlx90640To, 1, &mlx90640);
//  MLX90640_BadPixelsCorrection((&mlx90640)->outlierPixels, mlx90640To, 1, &mlx90640);
  time_end = micros();
  FPS = (float)1000000 / ((float)time_end-(float)time_start);
  Serial.printf("Waiting for data ready%d us\tGetFrame %dus\tCalculateTemperature %dus\nUpscale, Convert, write do LCD %dus\tDisplay text %dus\tTotal period %dms\n", time_1-time_start, time_2-time_1, time_3-time_2, time_4-time_3, time_5-time_4, time_end-time_5, (time_end-time_start)/1000);
}

void drawImage(uint16_t *colorArray, uint16_t columns, uint16_t rows, uint16_t x0, uint16_t y0, uint8_t pixelSize, uint8_t debug)
{
  char str_temp[6];
  uint16_t x, y;
  //tft.fillRect(0, 0, 8, 8, 0xFFFF);
  for(uint16_t i=0;i<columns*rows;i++)
  {
    x = (columns-(i%columns));    
    y = (i/columns);
    
  //if((((subPage+i+(y%2))%2)==1))

      if(debug==1)
      {
        if((i%columns)==0)
        {
          Serial.printf("\n");
          wdt_reset();
        }
        //dtostrf(colorArray[i], 2, 0, str_temp);  
        //Serial.printf("%s;", str_temp);
        Serial.printf("%.2x;", colorArray[i]);
      }
      Serial.printf("|x%dy%dc%x| ", x, y, colorArray[i]);      
      if(pixelSize==1)
      {
        tft.drawPixel(x+x0, y+y0, colorArray[i]);
      }else
      {
        tft.fillRect(x*pixelSize+x0, y*pixelSize+y0, pixelSize, pixelSize, colorArray[i]);
      }
        
  }
}
  
void Convert(float *frameTemperature, uint16_t *frameColor, uint16_t frameSize,
  const uint16_t *colorScale, uint16_t colorScaleSize,
  float minTemperature, float maxTemperature, uint8_t subPage)
{
  uint16_t i;
  //uint16_t constDegToScale;
  float constDegToScale;
  uint16_t colorIndex;

  if(minTemperature>15) minTemperature = 15;
  if(maxTemperature<50) maxTemperature = 50;

  constDegToScale = ((float)maxTemperature - (float)minTemperature) / ((float)colorScaleSize - (float)1);
//  Serial.printf("constDegToScale=%d maxTemperature=%d minTemperature=%d colorScaleSize=%d\n", (int)constDegToScale, maxTemperature, minTemperature, colorScaleSize);

  for(i=0; i<frameSize; i++)
  {
    if(((i/32)%2)==subPage)
    {
      if((frameTemperature[i] >= minTemperature) && (frameTemperature[i] <= maxTemperature))
      {
        colorIndex = (uint16_t)((frameTemperature[i] - (float)minTemperature) / constDegToScale);
      }else
      {
        colorIndex = 0;
      }
      //Serial.printf("%.4x", (int)frameTemperature[i]);
      //if((i%32)==0)
      //{
      //  Serial.printf("\n");
      //  wdt_reset();
      //}
      frameColor[i] = colorScale[colorIndex];
    }
  }
}

unsigned long readSensor(int value) {
/*  tft.fillScreen(ILI9341_BLACK);
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(1);
  tft.println("MLX90640");
  tft.setTextColor(ILI9341_YELLOW); tft.setTextSize(2);
  tft.println(value);
  tft.setTextColor(ILI9341_RED);    tft.setTextSize(3);
  tft.println(value, HEX);
  tft.println();
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(5);
  tft.println("Groop");
  tft.setTextSize(2);
  tft.println("I implore thee,");
  tft.setTextSize(1);
  tft.println("my foonting turlingdromes.");*/  
  return micros();// - start;
}

uint8_t I2C_scan(void)
{
  uint8_t detectedAddress = 0xFF;
    byte error, address;
  int nDevices;
 
  Serial.println("Scanning...");
 
  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    // The i2c_scanner uses the return value of
    // the Write.endTransmisstion to see if
    // a device did acknowledge to the address.
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
 
    if (error == 0)
    {
      Serial.print("I2C device found at address 0x");
      if (address<16)
        Serial.print("0");
      detectedAddress = address;
      Serial.print(address,HEX);
      Serial.println("  !");
 
      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("done\n");
    return(detectedAddress);
}

void printTemperaturesToConsole(float *mlx90640To, uint16_t pixelCount, uint8_t rowLength)
{
  for(uint16_t i=0;i<768;i++)
  {
    if((i%32)==0) Serial.printf("\n");
    Serial.printf("%d ", (uint8_t)mlx90640To[i]);
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
