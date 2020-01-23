#include <stdint.h>
#include <stdio.h>
#include "XPT2046.h"
#include "driver/spi_master.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"

static spi_device_handle_t psSPI_Device;

static spi_device_interface_config_t sDevCfg =
{

	.clock_speed_hz = 10*1000*1000,           //Clock out at 1 MHz
	.mode = 0,                                //SPI mode 0
	.spics_io_num = PIN_NUM_CS,               //CS pin
	.queue_size = 1,                          //We want to be able to queue 7 transactions at a time
};

enum cmd_bits
{
	ePD0 = 0, ePD1, eSER_DFR, eMODE,
	eA0, eA1, eA2, eS
};

#define CMD_X	(1<<eS)|(1<<eA0)
#define CMD_Y	(1<<eS)|(1<<eA1)

uint32_t XPT_2046_Init(void)
{
	esp_err_t i32Return;
	spi_transaction_t sTransaction;
	uint8_t i;

	/* Attach the LCD to the SPI bus */
	i32Return = spi_bus_add_device(HSPI_HOST, &sDevCfg, &psSPI_Device);
	ESP_ERROR_CHECK(i32Return);

	for(i=0; i<sizeof(sTransaction); i++)
	{
		((uint8_t*)&sTransaction)[i] = 0;
	}
    sTransaction.length = 8;                     			//Command is 8 bits
    //sTransaction.tx_buffer = &u8Cmd;               			//The data is the cmd itself
    sTransaction.cmd = CMD_X;	// X
	sTransaction.tx_data[0] = 0x00;
	sTransaction.tx_data[1] = 0x00;
	sTransaction.addr = 0;
	sTransaction.rxlength = 4;
//	i32Return = spi_device_polling_transmit(psSPI_Device, &sTransaction);  //Transmit!
//    assert(i32Return == ESP_OK);            				//Should have had no issues.
//	i32Return = spi_device_get_trans_result(psSPI_Device, &psTransaction, 500);
//    assert(i32Return == ESP_OK);            				//Should have had no issues.

///    sendGetXPT2046(0x90); // Must send first byte for initialisation (IRQ is most important)
    return(i32Return);
}


// 8bit cmd in
// wait 2ms
// 16 bit data out

unsigned int sendGetXPT2046 (uint8_t SPI_CMD)
{
	esp_err_t i32Return;
    unsigned int  SPIDataIn;
    spi_transaction_t sTransaction;
    uint8_t i;
    
    for(i=0; i<sizeof(sTransaction); i++)
    {
    	((uint8_t*)&sTransaction)[i] = 0;
    }
    sTransaction.length = 32;                     			//Command is 8 bits
    sTransaction.rxlength = 32;
    //sTransaction.tx_buffer = &u8Cmd;               			//The data is the cmd itself
    sTransaction.cmd = SPI_CMD;	// X
    sTransaction.tx_data[0] = 0x00;
    sTransaction.tx_data[1] = 0x00;
	sTransaction.addr = 0;

    i32Return = spi_device_transmit(psSPI_Device, &sTransaction);  //Transmit!
    assert(i32Return == ESP_OK);            				//Should have had no issues.
    
    SPIDataIn = (sTransaction.rx_data[0]) + (sTransaction.rx_data[1]<<8) + (sTransaction.rx_data[2]<<16) + (sTransaction.rx_data[3]<<24);
    //SPIDataIn >>=4;
    return (unsigned int) SPIDataIn;
}

unsigned int XPT_2046_GET_X(void)
{
    unsigned int x = 0;
    x = sendGetXPT2046(CMD_X);
//    x = ((x-Xmin)*LCD_WIDTH)/(Xmax-Xmin);
//    if(x<=0) x = 0;
//    if(x>=LCD_WIDTH) x = LCD_WIDTH;
    return (unsigned int) x;
    
}

unsigned int XPT_2046_GET_Y(void)
{
    unsigned int y = 0;
    y = sendGetXPT2046(CMD_Y);
//    y = ((y-Ymin)*LCD_HEIGHT)/(Ymax-Ymin);
//    if(y<=0) y = 0;
//    if(y>=LCD_HEIGHT) y = LCD_HEIGHT;
    return (unsigned int) y;
}
#if 0
// ============= Touch panel functions =========================================

#define USE_TOUCH TOUCH_TYPE_XPT2046

#if USE_TOUCH == TOUCH_TYPE_XPT2046
//-------------------------------------------------------
static int tp_get_data_xpt2046(uint8_t type, int samples)
{
	if (ts_spi == NULL) return 0;

	int n, result, val = 0;
	uint32_t i = 0;
	uint32_t vbuf[18];
	uint32_t minval, maxval, dif;

    if (samples < 3) samples = 1;
    if (samples > 18) samples = 18;

    // one dummy read
    result = touch_get_data(type);

    // read data
	while (i < 10) {
    	minval = 5000;
    	maxval = 0;
		// get values
		for (n=0;n<samples;n++) {
		    result = touch_get_data(type);
			if (result < 0) break;

			vbuf[n] = result;
			if (result < minval) minval = result;
			if (result > maxval) maxval = result;
		}
		if (result < 0) break;
		dif = maxval - minval;
		if (dif < 40) break;
		i++;
    }
	if (result < 0) return -1;

	if (samples > 2) {
		// remove one min value
		for (n = 0; n < samples; n++) {
			if (vbuf[n] == minval) {
				vbuf[n] = 5000;
				break;
			}
		}
		// remove one max value
		for (n = 0; n < samples; n++) {
			if (vbuf[n] == maxval) {
				vbuf[n] = 5000;
				break;
			}
		}
		for (n = 0; n < samples; n++) {
			if (vbuf[n] < 5000) val += vbuf[n];
		}
		val /= (samples-2);
	}
	else val = vbuf[0];

    return val;
}

//-----------------------------------------------
static int TFT_read_touch_xpt2046(int *x, int* y)
{
	int res = 0, result = -1;
	if (spi_lobo_device_select(ts_spi, 0) != ESP_OK) return 0;

    result = tp_get_data_xpt2046(0xB0, 3);  // Z; pressure; touch detect
	if (result <= 50) goto exit;

	// touch panel pressed
	result = tp_get_data_xpt2046(0xD0, 10);
	if (result < 0)  goto exit;

	*x = result;

	result = tp_get_data_xpt2046(0x90, 10);
	if (result < 0)  goto exit;

	*y = result;
	res = 1;
exit:
	spi_lobo_device_deselect(ts_spi);
	return res;
}
#endif

//=============================================
int TFT_read_touch(int *x, int* y, uint8_t raw)
{
    *x = 0;
    *y = 0;
	if (ts_spi == NULL) return 0;

	int result = -1;
    int X=0, Y=0;

#if USE_TOUCH == TOUCH_TYPE_XPT2046
    uint32_t tp_calx = TP_CALX_XPT2046;
    uint32_t tp_caly = TP_CALY_XPT2046;
   	result = TFT_read_touch_xpt2046(&X, &Y);
   	if (result == 0) return 0;
#endif

    if (raw) {
    	*x = X;
    	*y = Y;
    	return 1;
    }

    // Calibrate the result
	int tmp;
	int xleft   = (tp_calx >> 16) & 0x3FFF;
	int xright  = tp_calx & 0x3FFF;
	int ytop    = (tp_caly >> 16) & 0x3FFF;
	int ybottom = tp_caly & 0x3FFF;

	if (((xright - xleft) <= 0) || ((ybottom - ytop) <= 0)) return 0;

        int width = _width;
        int height = _height;
        X = ((X - xleft) * height) / (xright - xleft);
        Y = ((Y - ytop) * width) / (ybottom - ytop);

        if (X < 0) X = 0;
        if (X > height-1) X = height-1;
        if (Y < 0) Y = 0;
        if (Y > width-1) Y = width-1;

        switch (orientation) {
            case PORTRAIT:
                tmp = X;
                X = width - Y - 1;
                Y = tmp;
                break;
            case PORTRAIT_FLIP:
                tmp = X;
                X = Y;
                Y = height - tmp - 1;
                break;
            case LANDSCAPE_FLIP:
                X = height - X - 1;
                Y = width - Y - 1;
                break;
        }

	*x = X;
	*y = Y;
	return 1;
}
#endif
