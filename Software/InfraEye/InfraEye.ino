#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include "MLX90640_API.h"
#include "MLX90640_I2C_driver.h"
#include <Wire.h>

#define STMPE_CS 16
#define TFT_CS   0
#define TFT_DC   2//4//15
#define SD_CS    0

#define MLX90640_ADR 0x33
#define TA_SHIFT 8 //the default shift for a MLX90640 device in open air
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

uint16_t eeMLX90640[832];
paramsMLX90640 mlx90640;

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

  Serial.println("Get MLX90640 resolution:");
  result = MLX90640_GetCurResolution(0x33);
  Serial.println(result);
  

  // read diagnostics (optional but can help debug problems)
  uint8_t x = tft.readcommand8(ILI9341_RDMODE);
  Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDMADCTL);
  Serial.print("MADCTL Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDPIXFMT);
  Serial.print("Pixel Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDIMGFMT);
  Serial.print("Image Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(ILI9341_RDSELFDIAG);
  Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX); 
  
status = MLX90640_DumpEE (MLX90640_ADR, eeMLX90640);
status = MLX90640_ExtractParameters(eeMLX90640, &mlx90640);  
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
int status;

status = MLX90640_GetFrameData (MLX90640_ADR, mlx90640Frame);
tr = MLX90640_GetTa(mlx90640Frame, &mlx90640) - TA_SHIFT; //reflected temperature based on the sensor
Serial.printf("\nSubpage %d\n", MLX90640_GetSubPageNumber(mlx90640Frame));
dtostrf((double)tr, 4, 2, str_temp);
Serial.printf("Ambient temperature: %sÂ°C (%d)\n", str_temp, (int)tr);
wdt_reset();
MLX90640_CalculateTo(mlx90640Frame, &mlx90640, emissivity, tr, mlx90640To);

/*
Serial.printf("Frame:\n");
for(uint16_t i=0;i<834;i++)
{
  if((i%32)==0) Serial.printf("\n");  
  Serial.printf(" %.4x", mlx90640Frame[i]);
}*/

Serial.printf("Temperatures:\n");
for(uint16_t i=0;i<768;i++)
{
  uint16_t x, y;
  x = i%32;
  y = i/32;
  if((i%32)==0) Serial.printf("\n");
  /* 4 is mininum width, 2 is precision; float value is copied onto str_temp*/
  dtostrf((double)mlx90640To[i], 2, 0, str_temp);  
  Serial.printf("%s;", str_temp);
  //Serial.printf("x%dy%d ", x, y);  
  tft.drawPixel(x, y, (int)mlx90640To[i]);
}
Serial.printf("\n");
// -------------- subpage1?
delay(500);
wdt_reset();
tr = MLX90640_GetTa(mlx90640Frame, &mlx90640) - TA_SHIFT; //reflected temperature based on the sensor
Serial.printf("Subpage %d\n", MLX90640_GetSubPageNumber(mlx90640Frame));
wdt_reset();
delay(500);
wdt_reset();
//readSensor((int)mlx90640To[320]);
//tft.writePixel(120, 160, 1500);
//tft.fillRect(0, 0, 240, 240, 32000);

}

unsigned long readSensor(int value) {
  tft.fillScreen(ILI9341_BLACK);
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
  tft.println("my foonting turlingdromes.");  
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



