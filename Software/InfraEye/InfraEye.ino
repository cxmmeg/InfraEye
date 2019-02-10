//#include <Adafruit_GFX.h>
//#include <Adafruit_ILI9341.h>
#include "MLX90640_API.h"
#include "MLX90640_I2C_driver.h"
#include <Wire.h>

#define STMPE_CS 16
#define TFT_CS   0
#define TFT_DC   9//4//15
#define SD_CS    2

#define MLX90640_ADR 0x33
#define TA_SHIFT 8 //the default shift for a MLX90640 device in open air
//Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);

void setup()
{
  int result = -55;
  uint16_t data;
  uint8_t address;
  
  Serial.begin(115200);
  Wire.begin();

  delay(10);
  Serial.println("\nInfraEye boot...");
  address = I2C_scan();
  if(address != 0xFF)
  {
/*
  result = MLX90640_I2CRead(0x33, 0x8000, 1, &data);
  Serial.printf("Data: 0x%.4x Results: %d\n", data, result);
//  Serial.println(result);
//  Serial.println(data[0], HEX);
//  Serial.println(data[1], HEX);
  result = MLX90640_I2CRead(0x33, 0x800D, 1, &data);
  Serial.printf("Data: 0x%.4x Results: %d\n", data, result);
//  Serial.println(result);
//  Serial.println(data[0], HEX);
//  Serial.println(data[1], HEX);
  result = MLX90640_I2CRead(0x33, 0x800F, 1, &data);
  Serial.printf("Data: 0x%.4x Results: %d\n", data, result);*/

  Serial.println("Get MLX90640 resolution:");
  result = MLX90640_GetCurResolution(0x33);
  
  Serial.println(result);

  Serial.println("Set MLX90640 resolution:");
  result = MLX90640_SetResolution(0x33, 0);
  if(result==0)   Serial.println("Set MLX90640 resolution: OK");
  if(result==-1) Serial.println("Set MLX90640 resolution: Communication error");
  if(result==-2) Serial.println("Set MLX90640 resolution: Data not written!");
  //Serial.println(result);

  Serial.println("Get MLX90640 resolution:");
  result = MLX90640_GetCurResolution(0x33);  
  Serial.println(result);

char str_temp[6];

float emissivity = 0.95;
float tr;
static uint16_t eeMLX90640[832];
static uint16_t mlx90640Frame[834];
paramsMLX90640 mlx90640;
static float mlx90640To[768];
int status;
status = MLX90640_DumpEE (MLX90640_ADR, eeMLX90640);
status = MLX90640_ExtractParameters(eeMLX90640, &mlx90640);
status = MLX90640_GetFrameData (MLX90640_ADR, mlx90640Frame);
tr = MLX90640_GetTa(mlx90640Frame, &mlx90640) - TA_SHIFT; //reflected temperature based on the sensor
dtostrf((double)tr, 4, 2, str_temp);
Serial.printf("Ambient temperature: %s°C", str_temp);
Serial.printf("Ambient temperature: %d°C", (int)tr);
//ambient temperature
MLX90640_CalculateTo(mlx90640Frame, &mlx90640, emissivity, tr, mlx90640To);


for(uint16_t i=0;i<768;i++)
{
  if((i%32)==0) Serial.printf("\n");
  /* 4 is mininum width, 2 is precision; float value is copied onto str_temp*/
  dtostrf((double)mlx90640To[i], 4, 2, str_temp);
  //sprintf(temperature,"%s F", str_temp);  
  Serial.printf(" %s", str_temp);
}
  
//  tft.begin();
/*
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
*/  
  }
}


void loop(void) {
  for(uint8_t rotation=0; rotation<4; rotation++) {
    //tft.setRotation(rotation);
    //testText();
    delay(1000);
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
  tft.println();*/
  /*tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(5);
  tft.println("Groop");
  tft.setTextSize(2);
  tft.println("I implore thee,");
  tft.setTextSize(1);
  tft.println("my foonting turlingdromes.");
  tft.println("And hooptiously drangle me");
  tft.println("with crinkly bindlewurdles,");
  tft.println("Or I will rend thee");
  tft.println("in the gobberwarts");
  tft.println("with my blurglecruncheon,");
  tft.println("see if I don't!");*/
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


