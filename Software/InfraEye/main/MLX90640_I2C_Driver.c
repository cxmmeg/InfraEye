#include <stdint.h>
#include "driver/i2c.h"
#include "MLX90640_I2C_Driver.h"

#define WRITE_ADDR	(slaveAddr << 1)
#define READ_ADDR	((slaveAddr << 1) | 0x01)

#define I2C_MASTER_SCL_IO 13               /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO 12               /*!< gpio number for I2C master data  */
#define I2C_MASTER_FREQ_HZ 1000000

#define I2C_MASTER_RX_BUF_DISABLE 0
#define I2C_MASTER_TX_BUF_DISABLE 0

void MLX90640_I2CInit(void)
{ 
	int i2c_master_port = I2C_MASTER_NUM;
	i2c_cmd_handle_t cmd;

	i2c_config_t conf;
	conf.mode = I2C_MODE_MASTER;
	conf.sda_io_num = I2C_MASTER_SDA_IO;
	conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
	conf.scl_io_num = I2C_MASTER_SCL_IO;
	conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
	conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
	i2c_param_config(i2c_master_port, &conf);
	i2c_driver_install(i2c_master_port, conf.mode,
							  I2C_MASTER_RX_BUF_DISABLE,
							  I2C_MASTER_TX_BUF_DISABLE, 0);
}


int MLX90640_I2CRead(uint8_t slaveAddr, uint16_t startAddress, uint16_t nMemAddressRead, uint16_t *data)
{
	esp_err_t error = -1;
	i2c_cmd_handle_t cmd;
	esp_err_t i2c_ret;
	uint8_t tmp, i;
    
	ets_printf("---I2C Reading---\n");
	ets_printf("Read %d words from address 0x%x to read\n", nMemAddressRead, startAddress);

	// Write register address
	cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	error = i2c_master_write_byte(cmd, (slaveAddr << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, startAddress >> 8, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, startAddress &0x00FF, ACK_CHECK_EN);

	// Read data
	i2c_master_start(cmd);
	error = i2c_master_write_byte(cmd, (slaveAddr << 1) | I2C_MASTER_READ, ACK_CHECK_EN);
	i2c_master_read(cmd, (uint8_t*)data, nMemAddressRead*2, ACK_CHECK_DIS);
	//i2c_master_read_byte(cmd, &buff[1], ACK_CHECK_EN);
	//i2c_master_read_byte(cmd, &buff[0], ACK_CHECK_DIS);
	i2c_master_stop(cmd);
	i2c_ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, (1000 / portTICK_RATE_MS));	//"(# / portTICK_RATE_MS)"=maximum wait time. This task will be blocked until all the commands have been sent (not thread-safe - if you want to use one I2C port in different tasks you need to take care of multi-thread issues)
	i2c_cmd_link_delete(cmd);

	for(i=0;i<nMemAddressRead;i++)	// MSB and LSB not in correct order, swap them
	{
		tmp = data[i] & 0xFF;
		data[i] = (data[i]>>8) + (tmp<<8);
	}

	ets_printf("I2C Read data 0x%4x\n", data[0]);
	ets_printf("---I2C Reading End---\n");
	return error;   
} 

int MLX90640_I2CWrite(uint8_t slaveAddr, uint16_t writeAddress, uint16_t data)
{
	esp_err_t error = -1;
	i2c_cmd_handle_t cmd;
	esp_err_t i2c_ret;
	uint16_t dataCheck;

	ets_printf("---I2C Writing---\n");

	// Write register address
	cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	error = i2c_master_write_byte(cmd, (slaveAddr << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, writeAddress >> 8, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, writeAddress &0x00FF, ACK_CHECK_EN);
	ets_printf("I2C Write address 0x%.4x and data: 0x%.4x Error: %d\n", writeAddress, data, error);
//	i2c_master_stop(cmd);
//	i2c_ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, (1000 / portTICK_RATE_MS));	//"(# / portTICK_RATE_MS)"=maximum wait time. This task will be blocked until all the commands have been sent (not thread-safe - if you want to use one I2C port in different tasks you need to take care of multi-thread issues)
//	i2c_cmd_link_delete(cmd);

	// Write data
//	i2c_master_start(cmd);
	//ets_printf("I2C Read data 0x");
//	i2c_master_write(cmd, (uint8_t*)data, 2, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, data >> 8, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, data & 0x00FF, ACK_CHECK_DIS);
	i2c_master_stop(cmd);
	i2c_ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, (1000 / portTICK_RATE_MS));	//"(# / portTICK_RATE_MS)"=maximum wait time. This task will be blocked until all the commands have been sent (not thread-safe - if you want to use one I2C port in different tasks you need to take care of multi-thread issues)
	i2c_cmd_link_delete(cmd);

	MLX90640_I2CRead(slaveAddr, writeAddress, 1, &dataCheck);
    
	if ( dataCheck != data)
	{
		ets_printf("I2C Verify data: 0x%.4x VS 0x%.4x\n", dataCheck, data);
		return -2;
	}    
    
	ets_printf("---I2C Writing End---\n");
    return 0;
}

void MLX90640_I2CFreqSet(int freq)
{
    //Wire.setClock(freq*1000);
}
