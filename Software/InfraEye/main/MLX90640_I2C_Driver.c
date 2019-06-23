#include <stdint.h>
#include "driver/i2c.h"
#include "MLX90640_I2C_Driver.h"

#define WRITE_ADDR	(slaveAddr << 1)
#define READ_ADDR	((slaveAddr << 1) | 0x01)

#define I2C_MASTER_SCL_IO 19               /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO 20               /*!< gpio number for I2C master data  */
#define I2C_MASTER_FREQ_HZ 1000000

#define _I2C_NUMBER(num) I2C_NUM_##num
#define I2C_NUMBER(num) _I2C_NUMBER(num)

#define I2C_MASTER_NUM I2C_NUMBER(0) /*!< I2C port number for master dev */

void MLX90640_I2CInit()
{ 
	int i2c_master_port = I2C_MASTER_NUM;
	/*
	i2c_config_t conf;
	conf.mode = I2C_MODE_MASTER;
	conf.sda_io_num = I2C_MASTER_SDA_IO;
	conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
	conf.scl_io_num = I2C_MASTER_SCL_IO;
	conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
	conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
	i2c_param_config(i2c_master_port, &conf);
	return i2c_driver_install(i2c_master_port, conf.mode,
							  I2C_MASTER_RX_BUF_DISABLE,
							  I2C_MASTER_TX_BUF_DISABLE, 0);
	Arduino stuff*/
}


int MLX90640_I2CRead(uint8_t slaveAddr, uint16_t startAddress, uint16_t nMemAddressRead, uint16_t *data)
{
	int i = 0;
	uint8_t error = 0xFF;
	uint8_t bytes;
	uint8_t BufferSize;
	char c = 0;
    
//	Serial.println("---I2C Reading---");
//Serial.printf("S nMemAdd: %d\n", nMemAddressRead);
while(nMemAddressRead>0)
{	
//	Serial.printf("P1 nMemAdd: %d BufferSize: %d\n", nMemAddressRead, BufferSize);
	if(nMemAddressRead>=16)
	{
		nMemAddressRead = nMemAddressRead - 16;
		BufferSize = 16;
	}else
	{
		BufferSize = nMemAddressRead;
		nMemAddressRead = 0;
	}
//	Serial.printf("P2 nMemAdd: %d BufferSize: %d\n", nMemAddressRead, BufferSize);
    // Write register address
	/*
	Wire.beginTransmission(slaveAddr);	// 7bit I2C address
	Wire.write(startAddress >> 8);		// MSB of register address
	Wire.write(startAddress &0x00FF);	// LSB of register address

	startAddress = startAddress + BufferSize;
	error = Wire.endTransmission(false);
	*/
//	Serial.printf("I2C Write address 0x%.4x Error: %d\n", startAddress, error);	
	
    // Read data
////	bytes = Wire.requestFrom((int)slaveAddr, (int)BufferSize*2);
//	Serial.printf("I2C Read data 0x");	
////	while (Wire.available())
	{ // slave may send less than requested
////    		char c = Wire.read(); // receive a byte as character
		if((i&1)==0)
		{
        		*(data+(i>>1)) = (uint16_t)(c<<8);
		}else
		{
        		*(data+(i>>1)) = *(data+(i>>1)) + (uint16_t)c;
		}
		i++;
//		Serial.printf("%.2x", c);	
  	}
//	Serial.printf(" Bytes received: %d\n", bytes);	
}	
//	Serial.println("---I2C Reading End---");
	return error;   
} 

void MLX90640_I2CFreqSet(int freq)
{
    //Wire.setClock(freq*1000);
}

int MLX90640_I2CWrite(uint8_t slaveAddr, uint16_t writeAddress, uint16_t data)
{
	int error;
	static uint16_t dataCheck;
    
//	Serial.println("---I2C Writing---");
/*
	Wire.beginTransmission(slaveAddr);
   	Wire.write(writeAddress >> 8);
	Wire.write(writeAddress & 0x00FF);
   	Wire.write(data >> 8);
	Wire.write(data & 0x00FF);
	error = Wire.endTransmission();
	Arduino part*/
//	Serial.printf("I2C Write address 0x%.4x and data: 0x%.4x Error: %d\n", writeAddress, data, error);

	MLX90640_I2CRead(slaveAddr, writeAddress, 1, &dataCheck);
    
	if ( dataCheck != data)
	{
//		Serial.printf("I2C Verify data: 0x%.4x\n", dataCheck);
		return -2;
	}    
    
//	Serial.println("---I2C Writing End---");
    return 0;
}


