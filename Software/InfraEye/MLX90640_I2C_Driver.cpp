#include "MLX90640_I2C_Driver.h"
#include <Wire.h>
#include <Arduino.h>

#define WRITE_ADDR	(slaveAddr << 1)
#define READ_ADDR	((slaveAddr << 1) | 0x01)

void MLX90640_I2CIniti()
{ 
	Wire.begin();
    //Serial.println("I2C_Init");
}


int MLX90640_I2CRead(uint8_t slaveAddr, uint16_t startAddress, uint16_t nMemAddressRead, uint16_t *data)
{
	int i = 0;
	uint8_t error = 0xFF;
	uint8_t bytes;
	uint8_t BufferSize;
    
	Serial.println("---I2C Reading---");
Serial.printf("S nMemAdd: %d\n", nMemAddressRead);
while(nMemAddressRead>0)
{	
	Serial.printf("P1 nMemAdd: %d BufferSize: %d\n", nMemAddressRead, BufferSize);
	if(nMemAddressRead>=16)
	{
		nMemAddressRead = nMemAddressRead - 16;
		BufferSize = 16;
		wdt_reset();
	}else
	{
		BufferSize = nMemAddressRead;
		nMemAddressRead = 0;
	}
	Serial.printf("P2 nMemAdd: %d BufferSize: %d\n", nMemAddressRead, BufferSize);
    // Write register address
	Wire.beginTransmission(slaveAddr);	// 7bit I2C address
	Wire.write(startAddress >> 8);		// MSB of register address
	Wire.write(startAddress &0x00FF);	// LSB of register address
	error = Wire.endTransmission(false);
	Serial.printf("I2C Write address 0x%.4x Error: %d\n", startAddress, error);	
	
    // Read data
	bytes = Wire.requestFrom((int)slaveAddr, (int)BufferSize*2);
	Serial.printf("I2C Read data 0x");	
	while (Wire.available())
	{ // slave may send less than requested
    		char c = Wire.read(); // receive a byte as character
//		Serial.printf("pointer address: %x\n", data);
		if((i&1)==0)
		{
//			Serial.printf("DEBUG!!!! i=%d, pointer address: %x, data %.4x\n", i, data+(i>>1), (c<<8));
        		*(data+(i>>1)) = (uint16_t)(c<<8);
		}else
		{
//			Serial.printf("DEBUG!!!! i=%d, pointer address: %x, data %.4x\n", i, data+(i>>1), c);
        		*(data+(i>>1)) = *(data+(i>>1)) + (uint16_t)c;
		}
		i++;
		Serial.printf("%.2x", c);	
  	}
	Serial.printf(" Bytes received: %d\n", bytes);	
}	
	Serial.println("---I2C Reading End---");
	return error;   
} 

void MLX90640_I2CFreqSet(int freq)
{
//    i2c.frequency(1000*freq);
}

int MLX90640_I2CWrite(uint8_t slaveAddr, uint16_t writeAddress, uint16_t data)
{
	int error;
	static uint16_t dataCheck;
    
	Serial.println("---I2C Writing---");

	Wire.beginTransmission(slaveAddr);
   	Wire.write(writeAddress >> 8);
	Wire.write(writeAddress & 0x00FF);
   	Wire.write(data >> 8);
	Wire.write(data & 0x00FF);
	error = Wire.endTransmission();
	Serial.printf("I2C Write address 0x%.4x and data: 0x%.4x Error: %d\n", writeAddress, data, error);

	MLX90640_I2CRead(slaveAddr, writeAddress, 1, &dataCheck);
    
	if ( dataCheck != data)
	{
		Serial.printf("I2C Verify data: 0x%.4x\n", dataCheck);
		return -2;
	}    
    
	Serial.println("---I2C Writing End---");
    return 0;
}

