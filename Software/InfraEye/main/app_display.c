/*
 * app_display.c
 *
 * Created on: 		01-05-2019
 * Author: 			Martin Humaj
 *
 */


/************************************************************************************/
/* INCLUDES																			*/
/************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"
#include "esp32/rom/tjpgd.h"
#include "esp_log.h"
#include "config.h"
#include "app_display.h"

/************************************************************************************/
/* DEFINES																			*/
/************************************************************************************/

/************************************************************************************/
/* MODULE VARIABLES																	*/
/************************************************************************************/

DRAM_ATTR static const lcd_init_cmd_t app_disp_sILI_InitCmds[] =
{
    /* Power contorl B, power control = 0, DC_ENA = 1 */
    {0xCF, {0x00, 0x83, 0X30}, 3},
    /* Power on sequence control,
     * cp1 keeps 1 frame, 1st frame enable
     * vcl = 0, ddvdh=3, vgh=1, vgl=2
     * DDVDH_ENH=1
     */
    {0xED, {0x64, 0x03, 0X12, 0X81}, 4},
    /* Driver timing control A,
     * non-overlap=default +1
     * EQ=default - 1, CR=default
     * pre-charge=default - 1
     */
    {0xE8, {0x85, 0x01, 0x79}, 3},
    /* Power control A, Vcore=1.6V, DDVDH=5.6V */
    {0xCB, {0x39, 0x2C, 0x00, 0x34, 0x02}, 5},
    /* Pump ratio control, DDVDH=2xVCl */
    {0xF7, {0x20}, 1},
    /* Driver timing control, all=0 unit */
    {0xEA, {0x00, 0x00}, 2},
    /* Power control 1, GVDD=4.75V */
    {0xC0, {0x26}, 1},
    /* Power control 2, DDVDH=VCl*2, VGH=VCl*7, VGL=-VCl*3 */
    {0xC1, {0x11}, 1},
    /* VCOM control 1, VCOMH=4.025V, VCOML=-0.950V */
    {0xC5, {0x35, 0x3E}, 2},
    /* VCOM control 2, VCOMH=VMH-2, VCOML=VML-2 */
    {0xC7, {0xBE}, 1},
    /* Memory access contorl, MX=MY=0, MV=1, ML=0, BGR=1, MH=0 */
    {0x36, {0x28}, 1},
    /* Pixel format, 16bits/pixel for RGB/MCU interface */
    {0x3A, {0x55}, 1},
    /* Frame rate control, f=fosc, 70Hz fps */
    {0xB1, {0x00, 0x1B}, 2},
    /* Enable 3G, disabled */
    {0xF2, {0x08}, 1},
    /* Gamma set, curve 1 */
    {0x26, {0x01}, 1},
    /* Positive gamma correction */
    {0xE0, {0x1F, 0x1A, 0x18, 0x0A, 0x0F, 0x06, 0x45, 0X87, 0x32, 0x0A, 0x07, 0x02, 0x07, 0x05, 0x00}, 15},
    /* Negative gamma correction */
    {0XE1, {0x00, 0x25, 0x27, 0x05, 0x10, 0x09, 0x3A, 0x78, 0x4D, 0x05, 0x18, 0x0D, 0x38, 0x3A, 0x1F}, 15},
    /* Column address set, SC=0, EC=0xEF */
    {0x2A, {0x00, 0x00, 0x00, 0xEF}, 4},
    /* Page address set, SP=0, EP=0x013F */
    {0x2B, {0x00, 0x00, 0x01, 0x3f}, 4},
    /* Memory write */
    {0x2C, {0}, 0},
    /* Entry mode set, Low vol detect disabled, normal display */
    {0xB7, {0x07}, 1},
    /* Display function control */
    {0xB6, {0x0A, 0x82, 0x27, 0x00}, 4},
    /* Sleep out */
    {0x11, {0}, 0x80},
    /* Display on */
    {0x29, {0}, 0x80},
    {0, {0}, 0xff},
};

#if 0
//Reference the binary-included jpeg file
extern const uint8_t image_jpg_start[]   asm("_binary_image_jpg_start");
extern const uint8_t image_jpg_end[]     asm("_binary_image_jpg_end");

const char *TAG="ImageDec";
#endif

static spi_device_handle_t app_disp_psSPI_Device;
static spi_device_handle_t app_touch_psSPI_Device;

static spi_bus_config_t app_disp_sBusCfg =
{
	.miso_io_num = LCD_PIN_NUM_MISO,
	.mosi_io_num = LCD_PIN_NUM_MOSI,
	.sclk_io_num = LCD_PIN_NUM_CLK,
	.quadwp_io_num = -1,
	.quadhd_io_num = -1,
	.max_transfer_sz = app_disp_PARALLEL_LINES_D*DISP_COLUMNS_D*2 + 8
};

static spi_device_interface_config_t app_disp_sDevCfg =
{

	.clock_speed_hz = 60*1000*1000,           //Clock out at 26 MHz max for reliable reading. Flag SPI_DEVICE_NO_DUMMY set to ignore error stating that we have more than 80MHz, because we don't care about reading
	.mode = 0,                                //SPI mode 0
	.spics_io_num = LCD_PIN_NUM_CS,               //CS pin
	.queue_size = 7,                          //We want to be able to queue 7 transactions at a time
	.pre_cb = app_disp_vLCD_SPI_PreTransferCallback,  //Specify pre-transfer callback to handle D/C line
	.flags=SPI_DEVICE_NO_DUMMY				// Ignore errors
};

static spi_bus_config_t app_touch_sBusCfg =
{
	.miso_io_num = TOUCH_PIN_NUM_MISO,
	.mosi_io_num = TOUCH_PIN_NUM_MOSI,
	.sclk_io_num = TOUCH_PIN_NUM_CLK,
	.quadwp_io_num = -1,
	.quadhd_io_num = -1,
	.max_transfer_sz = 10*8
};

static spi_device_interface_config_t app_touch_sDevCfg =
{
	.clock_speed_hz = 125*1000,           //Clock out at 1 MHz
	.mode = 0,                                //SPI mode 0
	.spics_io_num = TOUCH_PIN_NUM_CS,               //CS pin
	.queue_size = 7                          //We want to be able to queue 7 transactions at a time	
};

static uint16_t* app_disp_pu16DispPixels;

// RGB format 565
DRAM_ATTR static const uint16_t colorPalete[] = {0x1,0x2,0x3,0x4,0x5,0x5,0x6,0x6,0x7,0x7,0x8,0x8,0x9,0x9,0xa,0xa,0xa,0xb,0xb,0xb,0xc,0xc,0xc,0xc,0xd,0x80d,0x80d,0x80e,0x80e,0x80e,0x80e,0x80e,0x80e,0x100f,0x100f,0x100f,0x100f,0x100f,0x180f,0x1810,0x1810,0x1810,0x2010,0x2010,0x2010,0x2010,0x2811,0x2811,0x2811,0x2811,0x3011,0x3011,0x3011,0x3011,0x3811,0x3812,0x3812,0x3812,0x3812,0x3812,0x4012,0x4012,0x4012,0x4012,0x4012,0x4812,0x4812,0x4812,0x4812,0x4812,0x5012,0x5013,0x5013,0x5013,0x5813,0x5813,0x5813,0x5813,0x5813,0x6013,0x6013,0x6013,0x6013,0x6813,0x6813,0x6813,0x6813,0x6813,0x7013,0x7013,0x7013,0x7013,0x7013,0x7813,0x7813,0x7813,0x7813,0x7813,0x8013,0x8013,0x8013,0x8013,0x8013,0x8813,0x8813,0x8813,0x8813,0x8813,0x9013,0x9013,0x9013,0x9013,0x9813,0x9813,0x9813,0x9813,0x9813,0x9813,0xa013,0xa013,0xa013,0xa013,0xa013,0xa013,0xa813,0xa813,0xa813,0xa813,0xa813,0xa813,0xa813,0xb013,0xb013,0xb012,0xb012,0xb012,0xb012,0xb012,0xb012,0xb012,0xb812,0xb832,0xb832,0xb832,0xb832,0xb832,0xb832,0xb832,0xb832,0xb832,0xc032,0xc032,0xc052,0xc052,0xc051,0xc051,0xc051,0xc071,0xc071,0xc071,0xc071,0xc071,0xc891,0xc890,0xc890,0xc890,0xc890,0xc8b0,0xc8b0,0xc8b0,0xc8b0,0xc8cf,0xc8cf,0xc8cf,0xd0cf,0xd0cf,0xd0ee,0xd0ee,0xd0ee,0xd0ee,0xd10e,0xd10d,0xd10d,0xd10d,0xd12d,0xd12c,0xd12c,0xd92c,0xd94c,0xd94c,0xd94b,0xd96b,0xd96b,0xd96a,0xd96a,0xd98a,0xd989,0xd989,0xd988,0xd9a8,0xd9a8,0xd9a7,0xe1a7,0xe1c6,0xe1c6,0xe1c6,0xe1c5,0xe1e5,0xe1e4,0xe1e4,0xe1e4,0xe203,0xe203,0xe203,0xe223,0xe223,0xe222,0xe222,0xe242,0xe242,0xea42,0xea62,0xea61,0xea61,0xea61,0xea61,0xea61,0xea81,0xea81,0xea81,0xea81,0xeaa1,0xeaa1,0xeaa1,0xeac1,0xeac0,0xeac0,0xeac0,0xeae0,0xeae0,0xeae0,0xeae0,0xeae0,0xeb00,0xeb00,0xeb00,0xf300,0xf320,0xf320,0xf320,0xf320,0xf320,0xf340,0xf340,0xf340,0xf340,0xf340,0xf360,0xf360,0xf360,0xf360,0xf380,0xf380,0xf380,0xf380,0xf3a0,0xf3a0,0xf3a0,0xf3a0,0xf3c0,0xf3c0,0xf3c0,0xf3e0,0xf3e0,0xf3e0,0xf400,0xf400,0xf400,0xf400,0xf420,0xf420,0xf420,0xfc20,0xfc40,0xfc40,0xfc40,0xfc40,0xfc40,0xfc60,0xfc60,0xfc60,0xfc60,0xfc60,0xfc80,0xfc80,0xfc80,0xfc80,0xfca0,0xfca0,0xfca0,0xfcc0,0xfcc0,0xfcc0,0xfce0,0xfce0,0xfce0,0xfd00,0xfd00,0xfd00,0xfd00,0xfd20,0xfd20,0xfd20,0xfd40,0xfd40,0xfd40,0xfd60,0xfd60,0xfd60,0xfd60,0xfd80,0xfd80,0xfd80,0xfd80,0xfda0,0xfda0,0xfda0,0xfdc0,0xfdc0,0xfdc0,0xfdc0,0xfdc0,0xfde0,0xfde0,0xfde0,0xfe00,0xfe00,0xfe00,0xfe00,0xfe20,0xfe20,0xfe20,0xfe20,0xfe40,0xfe40,0xfe40,0xfe40,0xfe40,0xfe60,0xfe60,0xfe60,0xfe60,0xfe60,0xfe80,0xfe80,0xfe81,0xfea1,0xfea1,0xfea1,0xfea1,0xfec1,0xfec1,0xfec1,0xfec1,0xfec2,0xfee2,0xfee2,0xfee2,0xfee3,0xfee3,0xfee3,0xff04,0xff04,0xff04,0xff04,0xff05,0xff25,0xff25,0xff26,0xff26,0xff27,0xff27,0xff47,0xff48,0xff48,0xff49,0xff49,0xff6a,0xff6a,0xff6a,0xff6b,0xff6b,0xff6c,0xff6c,0xff8d,0xff8d,0xff8e,0xff8e,0xff8f,0xff90,0xff90,0xff91,0xff91,0xffb2,0xffb2,0xffb3,0xffb3,0xffb4,0xffb4,0xffb5,0xffb5,0xffb6,0xffb6,0xffd7,0xffd7,0xffd8,0xffd8,0xffd8,0xffd9,0xffd9,0xffda,0xffda,0xffdb,0xfffb,0xfffb,0xfffc,0xfffc,0xfffd,0xfffd,0xfffd,0xfffe,0xfffe,0xfffe};


#if ENABLE_DISP_FRAME_RATE_MEASURE_D
	static uint32_t app_disp_u32FrameTime_ms = 0uL;
#endif

/************************************************************************************/
/* FUNCTION PROTOTYPES																*/
/************************************************************************************/

static void app_disp_vLCD_Init(spi_device_handle_t psSPI_Device);

static void app_disp_vLCD_Cmd(spi_device_handle_t psSPI_Device, const uint8_t u8Cmd);

static void app_disp_vLCD_SendData(spi_device_handle_t psSPI_Device, const uint8_t *pu8Data, int intLength);

static void app_disp_vSendLines(spi_device_handle_t psSPI_Device, int intPosY, uint16_t *pu16LineData);

static void app_disp_vSendLineFinish(spi_device_handle_t spi);

/************************************************************************************/
/* FUNCTION DEFINITIONS																*/
/************************************************************************************/

void app_disp_vInitialize(void)
{

	esp_err_t i32Return;
	uint32_t u32_r;
	uint8_t cmd;
	uint32_t u32_count = 0;

	ets_printf("------------------------ Start -------------------------------\n");
	/* Initialize the SPI bus */
	i32Return = spi_bus_initialize(VSPI_HOST, &app_disp_sBusCfg, 1);
	ESP_ERROR_CHECK(i32Return);
	ets_printf("LCD Bus initialized -----------------\n");

	/* Attach the LCD to the SPI bus */
	i32Return = spi_bus_add_device(VSPI_HOST, &app_disp_sDevCfg, &app_disp_psSPI_Device);
	ESP_ERROR_CHECK(i32Return);
	ets_printf("------------------------ LCD device added to bus -------------\n");
#if 1
	i32Return = spi_bus_initialize(HSPI_HOST, &app_touch_sBusCfg, 2);
	ESP_ERROR_CHECK(i32Return);
	ets_printf("Touch Bus initialized ---------------\n");
	
	/* Attach the touch sensor to the SPI bus */
	i32Return = spi_bus_add_device(HSPI_HOST, &app_touch_sDevCfg, &app_touch_psSPI_Device);
	ESP_ERROR_CHECK(i32Return);
	ets_printf("------------------------ Touch device added to bus -----------\n");
	ets_printf("------------------------ End ---------------------------------\n");
#endif
	/* Initialize the LCD */
	app_disp_vLCD_Init(app_disp_psSPI_Device);
	heap_caps_print_heap_info(MALLOC_CAP_DMA);
	app_disp_pu16DispPixels = heap_caps_malloc(DISP_COLUMNS_D*DISP_ROWS_D*sizeof(uint16_t), MALLOC_CAP_DMA);
	heap_caps_print_heap_info(MALLOC_CAP_DMA);
	assert(app_disp_pu16DispPixels != NULL);

}

uint32_t app_touch_DoMagic(uint8_t u8_cmd)
{
	esp_err_t i32Return = 0;
	
	uint8_t au8_rxdata[4];
	uint8_t au8_txdata[4] = {0xA5, 0xB6, 0xC7, 0xD8};

	//app_disp_vLCD_Cmd(app_touch_psSPI_Device, u8_cmd);

	spi_transaction_t sTransaction;
	memset(&sTransaction, 0, sizeof(sTransaction)); 		//Zero out the transaction
	sTransaction.length = 8*4;
	sTransaction.tx_buffer = au8_txdata;
	sTransaction.tx_data[0] = u8_cmd;
	sTransaction.rx_buffer = au8_rxdata;
 	sTransaction.flags = SPI_TRANS_USE_RXDATA|SPI_TRANS_USE_TXDATA;
	i32Return = spi_device_polling_transmit(app_touch_psSPI_Device, &sTransaction);  //Transmit!
	assert(i32Return == ESP_OK);            				//Should have had no issues.

	return sTransaction.rx_data[0]+(sTransaction.rx_data[1]<<8)+(sTransaction.rx_data[2]<<16)+(sTransaction.rx_data[3]<<24);
}

/* Send a command to the LCD. Uses spi_device_polling_transmit, which waits
 * until the transfer is complete.
 *
 * Since command transactions are usually small, they are handled in polling
 * mode for higher speed. The overhead of interrupt transactions is more than
 * just waiting for the transaction to complete.
 */
static void app_disp_vLCD_Cmd(spi_device_handle_t psSPI_Device, const uint8_t u8Cmd)
{
    esp_err_t i32Return;
    spi_transaction_t sTransaction;

    memset(&sTransaction, 0, sizeof(sTransaction)); 		//Zero out the transaction
    sTransaction.length = 8;                     			//Command is 8 bits
    sTransaction.tx_buffer = &u8Cmd;               			//The data is the cmd itself
    sTransaction.user = (void*)0;                			//D/C needs to be set to 0
    i32Return = spi_device_polling_transmit(psSPI_Device, &sTransaction);  //Transmit!
    assert(i32Return == ESP_OK);            				//Should have had no issues.
}

/* Send data to the LCD. Uses spi_device_polling_transmit, which waits until the
 * transfer is complete.
 *
 * Since data transactions are usually small, they are handled in polling
 * mode for higher speed. The overhead of interrupt transactions is more than
 * just waiting for the transaction to complete.
 */
static void app_disp_vLCD_SendData(spi_device_handle_t psSPI_Device, const uint8_t *pu8Data, int intLength)
{
    esp_err_t i32Return;
    spi_transaction_t sTransaction;

    if (intLength == 0)
    {
        //no need to send anything
    	return;
    }

    memset(&sTransaction, 0, sizeof(sTransaction));     //Zero out the transaction
    sTransaction.length = intLength*8;                 	//Len is in bytes, transaction length is in bits.
    sTransaction.tx_buffer = pu8Data;               	//Data
    sTransaction.user = (void*)1;                		//D/C needs to be set to 1
    i32Return = spi_device_polling_transmit(psSPI_Device, &sTransaction);  //Transmit!

    assert(i32Return == ESP_OK);            //Should have had no issues.
}

//This function is called (in irq context!) just before a transmission starts. It will
//set the D/C line to the value indicated in the user field.
void app_disp_vLCD_SPI_PreTransferCallback(spi_transaction_t *psTransaction)
{
    int intDC = (int)psTransaction->user;

    gpio_set_level(LCD_PIN_NUM_DC, intDC);
}

uint32_t app_disp_u32LCD_GetID(void)
{
    // get_id command
    app_disp_vLCD_Cmd(app_disp_psSPI_Device, 0x04);

    spi_transaction_t sTransaction;
    memset(&sTransaction, 0, sizeof(sTransaction));
    sTransaction.length = 8*3;
    sTransaction.flags = SPI_TRANS_USE_RXDATA;
    sTransaction .user = (void*)1;

    esp_err_t ret = spi_device_polling_transmit(app_disp_psSPI_Device, &sTransaction);
    assert( ret == ESP_OK );

    return *(uint32_t*)sTransaction.rx_data;
}

//Initialize the display
static void app_disp_vLCD_Init(spi_device_handle_t psSPI_Device)
{
    int intCmd = 0;
    const lcd_init_cmd_t* psLCD_InitCmds;

    // Initialize non-SPI GPIOs
    gpio_set_direction(LCD_PIN_NUM_DC, GPIO_MODE_OUTPUT);
    gpio_set_direction(LCD_PIN_NUM_RST, GPIO_MODE_OUTPUT);
//    gpio_set_direction(PIN_NUM_BCKL, GPIO_MODE_OUTPUT);

    // Reset the display
    gpio_set_level(LCD_PIN_NUM_RST, 0);
    vTaskDelay(100 / portTICK_RATE_MS);
    gpio_set_level(LCD_PIN_NUM_RST, 1);
    vTaskDelay(100 / portTICK_RATE_MS);

    psLCD_InitCmds = app_disp_sILI_InitCmds;

    //Send all the commands
    while (psLCD_InitCmds[intCmd].databytes != 0xFF)
    {
        app_disp_vLCD_Cmd(psSPI_Device, psLCD_InitCmds[intCmd].cmd);

        app_disp_vLCD_SendData(psSPI_Device, psLCD_InitCmds[intCmd].data, psLCD_InitCmds[intCmd].databytes & 0x1F);
        if (psLCD_InitCmds[intCmd].databytes & 0x80)
        {
            vTaskDelay(100 / portTICK_RATE_MS);
        }
        intCmd ++;
    }

    ///Enable backlight
//    gpio_set_level(PIN_NUM_BCKL, 0);
}


/* To send a set of lines we have to send a command, 2 data bytes, another command, 2 more data bytes and another command
 * before sending the line data itself; a total of 6 transactions. (We can't put all of this in just one transaction
 * because the D/C line needs to be toggled in the middle.)
 * This routine queues these commands up as interrupt transactions so they get
 * sent faster (compared to calling spi_device_transmit several times), and at
 * the mean while the lines for next transactions can get calculated.
 */
static void app_disp_vSendLines(spi_device_handle_t psSPI_Device, int intPosY, uint16_t *pu16LineData)
{
    esp_err_t i32Return;
    int x;
    //Transaction descriptors. Declared static so they're not allocated on the stack; we need this memory even when this
    //function is finished because the SPI driver needs access to it even while we're already calculating the next line.
    static spi_transaction_t asTransaction[6];

    //In theory, it's better to initialize trans and data only once and hang on to the initialized
    //variables. We allocate them on the stack, so we need to re-init them each call.
    for (x = 0; x < 6; x++)
    {
        memset(&asTransaction[x], 0, sizeof(spi_transaction_t));
        if ((x & 1) == 0)
        {
            //Even transfers are commands
            asTransaction[x].length = 8;
            asTransaction[x].user = (void*)0;
        }
        else
        {
            //Odd transfers are data
            asTransaction[x].length = 8*4;
            asTransaction[x].user = (void*)1;
        }
        asTransaction[x].flags = SPI_TRANS_USE_TXDATA;
    }

    asTransaction[0].tx_data[0] = 0x2A;           //Column Address Set
    asTransaction[1].tx_data[0] = 0;              //Start Col High
    asTransaction[1].tx_data[1] = 0;              //Start Col Low
    asTransaction[1].tx_data[2] = (DISP_COLUMNS_D) >> 8;     //End Col High
    asTransaction[1].tx_data[3] = (DISP_COLUMNS_D) & 0xFF;   //End Col Low
    asTransaction[2].tx_data[0] = 0x2B;           //Page address set
    asTransaction[3].tx_data[0] = intPosY >> 8;        //Start page high
    asTransaction[3].tx_data[1] = intPosY & 0xFF;      //start page low
    asTransaction[3].tx_data[2] = (intPosY + app_disp_PARALLEL_LINES_D) >> 8;    	//end page high
    asTransaction[3].tx_data[3] = (intPosY + app_disp_PARALLEL_LINES_D) & 0xFF;  	//end page low
    asTransaction[4].tx_data[0] = 0x2C;           //memory write
    asTransaction[5].tx_buffer = pu16LineData;    //finally send the line data
    asTransaction[5].length = DISP_COLUMNS_D*2*8*app_disp_PARALLEL_LINES_D;          //Data length, in bits
    asTransaction[5].flags = 0; //undo SPI_TRANS_USE_TXDATA flag

    //Queue all transactions.
    for (x = 0; x < 6; x++)
    {
    	i32Return = spi_device_queue_trans(psSPI_Device, &asTransaction[x], portMAX_DELAY);
        assert(i32Return == ESP_OK);
    }

    //When we are here, the SPI driver is busy (in the background) getting the transactions sent. That happens
    //mostly using DMA, so the CPU doesn't have much to do here. We're not going to wait for the transaction to
    //finish because we may as well spend the time calculating the next line. When that is done, we can call
    //send_line_finish, which will wait for the transfers to be done and check their status.
}


static void app_disp_vSendLineFinish(spi_device_handle_t psSPI_Device)
{
    spi_transaction_t *psTransaction;
    esp_err_t i32Return;

    //Wait for all 6 transactions to be done and get back the results.
    for (int x = 0; x < 6; x++)
    {
        i32Return = spi_device_get_trans_result(psSPI_Device, &psTransaction, portMAX_DELAY);

        assert(i32Return == ESP_OK);
        //We could inspect rtrans now if we received any info back. The LCD is treated as write-only, though.
    }
}


void app_disp_vRunDisplayTask(void)
{

	static uint16_t u16DispColumnsCounter = 0u;
	static uint8_t u8FirstTime = 1u;
	static uint32_t u32StartTime = 0uL;
	static uint32_t u32EndTime = 0uL;

	uint16_t* pu16Pixels;

	if(u8FirstTime == 0u)
	{
		/* Wait for finishing previous transaction */
		app_disp_vSendLineFinish(app_disp_psSPI_Device);
	}
	else
	{
#if ENABLE_DISP_FRAME_RATE_MEASURE_D
		/* Start to measure time */
		u32StartTime = (uint32_t)xTaskGetTickCount();
#endif
		u8FirstTime = 0u;
	}

	if(u16DispColumnsCounter == DISP_COLUMNS_D)
	{
#if ENABLE_DISP_FRAME_RATE_MEASURE_D
		/* Stop measuring time after one frame was displayed */
		u32EndTime = (uint32_t)xTaskGetTickCount();
		/* Calculate elapsed time */
		app_disp_u32FrameTime_ms = u32EndTime - u32StartTime;
#endif
		u16DispColumnsCounter = 0u;
#if ENABLE_DISP_FRAME_RATE_MEASURE_D
		/* Start to measure time */
		u32StartTime = (uint32_t)xTaskGetTickCount();
#endif
	}

	pu16Pixels = app_disp_pu16DispPixels + (u16DispColumnsCounter*DISP_ROWS_D);

	/* Display section */
	app_disp_vSendLines(app_disp_psSPI_Device, u16DispColumnsCounter, pu16Pixels);
	/* Increment column counter */
	u16DispColumnsCounter += app_disp_PARALLEL_LINES_D;

}

void app_disp_vSetRectangleColour(uint16_t u16ColPos, uint16_t u16RowPos, uint16_t u16ColLength, uint16_t u16RowLength, uint16_t u16Colour)
{
	uint16_t u16ColIterator;
	uint16_t u16RowIterator;
	uint16_t* pu16Pixels;

	for(u16ColIterator = u16ColPos; u16ColIterator < (u16ColPos + u16ColLength); u16ColIterator++)
	{
		for(u16RowIterator = u16RowPos; u16RowIterator < (u16RowPos + u16RowLength); u16RowIterator++)
		{
			pu16Pixels = app_disp_pu16DispPixels + ((u16ColIterator * DISP_ROWS_D) + u16RowIterator);
			*pu16Pixels = u16Colour;
		}
	}
}

void app_disp_vSetPixelColour(uint16_t u16ColPos, uint16_t u16RowPos, uint16_t u16Colour)
{
	uint16_t* pu16Pixels;

	pu16Pixels = app_disp_pu16DispPixels + ((u16ColPos * DISP_ROWS_D) + u16RowPos);
	*pu16Pixels = u16Colour;
}

#if ENABLE_DISP_FRAME_RATE_MEASURE_D
uint16_t app_disp_u16GetFrameRate(void)
{
	return(1000/app_disp_u32FrameTime_ms);
}
#endif

void LCD_Convert_u16(uint16_t *frameTemperature, uint16_t *frameColor, uint16_t frameSize,
  uint16_t minTemperature, uint16_t maxTemperature)
{
	uint16_t i;
	float constDegToScale;
	uint16_t colorIndex;
	uint16_t colorScaleSize = 403;
	uint16_t color_converted;
	uint8_t cmd;
	uint32_t u32_r;

	constDegToScale = ((float)maxTemperature - (float)minTemperature) / ((float)colorScaleSize - (float)1);

	for(i = 0; i < frameSize; i++)
	{
		{
			if(frameTemperature[i] < minTemperature)
			{
				colorIndex = 0;
			}
			else if(frameTemperature[i] > maxTemperature)
			{
				colorIndex = colorScaleSize - 1;
			}
			else
			{
				colorIndex = (uint16_t)(((float)frameTemperature[i] - (float)minTemperature) / constDegToScale);
			}

			color_converted = ((colorPalete[colorIndex]&0xFF00)>>8) + ((colorPalete[colorIndex]&0x00FF)<<8);
			frameColor[i] = color_converted;
		}
	}
	for(cmd = 0x8; cmd<0x10; cmd++)
		{
			u32_r = app_touch_DoMagic((cmd<<4));
			ets_printf("Touch 0x%02x:0x%04x\n", (cmd<<4), u32_r);
			if(cmd == 0x8) ets_printf("\tPressure:%d\n", (u32_r>>9)&0x0FFF);
			if(cmd == 0x9) ets_printf("\tX:%d\n", (u32_r>>9)&0x0FFF);
			if(cmd == 0xd) ets_printf("\tY:%d\n", (u32_r>>9)&0x0FFF);
		}
		ets_printf("\n");
}
