#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "MLX90640_API.h"
#include "MLX90640_I2C_driver.h"
#include <Wire.h>
//#include "UpscaleImage.h"

#define STMPE_CS 16
#define TFT_CS   0
#define TFT_DC   2//4//15
#define SD_CS    0
#define RAW_IMAGE

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
  
  Serial.begin(115200);
  tft.begin();
  Wire.begin();

  delay(10);
  Serial.println("\nInfraEye boot...");
  address = I2C_scan();

  MLX90640_SetResolution(MLX90640_ADR, 0x03);
  Serial.println("Get MLX90640 resolution:");
  result = MLX90640_GetCurResolution(0x33);
  Serial.println(result);
  MLX90640_SetRefreshRate(MLX90640_ADR, 0x01);
  Wire.setClock(400000);
  
  // read diagnostics (optional but can help debug problems)
/*  uint8_t x = tft.readcommand8(ILI9341_RDMODE);
  Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDMADCTL);
  Serial.print("MADCTL Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDPIXFMT);
  Serial.print("Pixel Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDIMGFMT);
  Serial.print("Image Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDSELFDIAG);
  Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX); 
  */
status = MLX90640_DumpEE (MLX90640_ADR, eeMLX90640);
status = MLX90640_ExtractParameters(eeMLX90640, &mlx90640);  
tft.fillRect(0, 0, 240, 320, 0x0000);
}


void loop(void)
{
  int result = -55;
  uint16_t data;
  uint8_t address;
char str_temp[6];
float emissivity = 0.95;
float tr;
static uint16_t mlx90640Frame[834];
static float mlx90640To[768];
static float upscaledTo[11408];
int status;
uint16_t frameColor[768];
float min_t = 300;
float max_t = -40;

status = MLX90640_GetFrameData (MLX90640_ADR, mlx90640Frame);
Serial.printf("Status:%d\n", status);
tr = MLX90640_GetTa(mlx90640Frame, &mlx90640) - TA_SHIFT; //reflected temperature based on the sensor
Serial.printf("\nSubpage %d\n", MLX90640_GetSubPageNumber(mlx90640Frame));
dtostrf((double)tr, 4, 2, str_temp);
Serial.printf("Ambient temperature: %sÂ°C (%d)\n", str_temp, (int)tr);
wdt_reset();
MLX90640_CalculateTo(mlx90640Frame, &mlx90640, emissivity, tr, mlx90640To);
MLX90640_BadPixelsCorrection((&mlx90640)->brokenPixels, mlx90640To, 1, &mlx90640);
MLX90640_BadPixelsCorrection((&mlx90640)->outlierPixels, mlx90640To, 1, &mlx90640);

//UpscaleImage(mlx90640To, upscaledTo);
/*
Serial.printf("Frame:\n");
for(uint16_t i=0;i<834;i++)
{
  if((i%32)==0) Serial.printf("\n");  
  Serial.printf(" %.4x", mlx90640Frame[i]);
}*/

for(uint16_t i=0;i<768;i++)
{
  if(mlx90640To[i] > max_t) max_t = mlx90640To[i];
  if(mlx90640To[i] < min_t) min_t = mlx90640To[i];
}

Convert(mlx90640To, frameColor, 768,
  colorPalete, 403,
  min_t, max_t);

Serial.printf("Temperatures:\n");

for(uint16_t i=0;i<768;i++)
{
  uint16_t x, y;
  x = (i%32)*8;
  y = (i/32)*8;
//  if((i%32)==0) Serial.printf("\n");
// 4 is mininum width, 2 is precision; float value is copied onto str_temp
//  dtostrf((double)mlx90640To[i], 2, 0, str_temp);  
//  Serial.printf("%s;", str_temp);
  //Serial.printf("x%dy%d ", x, y);  
  //tft.drawPixel(x, y, (int)mlx90640To[i]);
  //tft.drawPixel(x, y, frameColor[i]);
  tft.fillRect(x, y, 8, 8, frameColor[i]);
}
tft.fillRect(0, 200, 240, 80, 0x0000);
tft.setCursor(10, 200);
tft.setTextColor(ILI9341_WHITE);  tft.setTextSize(3);
dtostrf((double)max_t, 3, 0, str_temp);
tft.printf("Tmax=%s°C", str_temp);
tft.setCursor(10, 240);
dtostrf((double)min_t, 3, 0, str_temp);
tft.printf("Tmin=%s°C", str_temp);
/*
for(uint16_t i=0;i<2852;i++)
{
  uint16_t x, y;

  x = i%320;
  y = i/320;
//  tft.drawPixel(x, y, (int)upscaledTo[i]);
}
*/
Serial.printf("\n");
// -------------- subpage1?
//delay(100);
wdt_reset();
tr = MLX90640_GetTa(mlx90640Frame, &mlx90640) - TA_SHIFT; //reflected temperature based on the sensor
Serial.printf("Subpage %d\n", MLX90640_GetSubPageNumber(mlx90640Frame));
wdt_reset();
//delay(100);
//wdt_reset();
//readSensor((int)mlx90640To[320]);
//tft.writePixel(120, 160, 1500);
//tft.fillRect(0, 0, 240, 240, 32000);

}

void Convert(float *frameTemperature, uint16_t *frameColor, uint16_t frameSize,
  const uint16_t *colorScale, uint16_t colorScaleSize,
  float minTemperature, float maxTemperature)
{
  uint16_t i;
  //uint16_t constDegToScale;
  float constDegToScale;
  uint16_t colorIndex;

  //if(minTemperature>15) minTemperature = 15;
  //if(maxTemperature<50) maxTemperature = 50;

  constDegToScale = ((float)maxTemperature - (float)minTemperature) / ((float)colorScaleSize - (float)1);
  Serial.printf("constDegToScale=%d maxTemperature=%d minTemperature=%d colorScaleSize=%d\n", (int)constDegToScale, maxTemperature, minTemperature, colorScaleSize);

  for(i=0; i<frameSize; i++)
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
  Serial.printf("frameColor=%d frameTemperature[320]=%d\n", frameColor[320], (int)frameTemperature[320]);
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



