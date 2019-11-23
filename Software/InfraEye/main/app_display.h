/*
 * app_display.c
 *
 * Created on: 		01-05-2019
 * Author: 			Martin Humaj
 *
 */

#ifndef APP_DISPLAY_H_
#define APP_DISPLAY_H_

/************************************************************************************/
/* INCLUDES																			*/
/************************************************************************************/

#include <stdint.h>
#include "esp_err.h"
#include "driver/spi_master.h"

/************************************************************************************/
/* DEFINES																			*/
/************************************************************************************/

/*
 This code displays some fancy graphics on the 320x240 LCD on an ESP-WROVER_KIT board.
 This example demonstrates the use of both spi_device_transmit as well as
 spi_device_queue_trans/spi_device_get_trans_result and pre-transmit callbacks.

 Some info about the ILI9341/ST7789V: It has an C/D line, which is connected to a GPIO here. It expects this
 line to be low for a command and high for data. We use a pre-transmit callback here to control that
 line: every transaction has as the user-definable argument the needed state of the D/C line and just
 before the transaction is sent, the callback will set this line to the correct state.
*/

#define PIN_NUM_MISO 25
#define PIN_NUM_MOSI 23
#define PIN_NUM_CLK  19
#define PIN_NUM_CS   22

#define PIN_NUM_DC   21
#define PIN_NUM_RST  18
#define PIN_NUM_BCKL 5

/* Configure display frame rate measurement */
#define ENABLE_DISP_FRAME_RATE_MEASURE_D	(1)

//To speed up transfers, every SPI transfer sends a bunch of lines. This define specifies how many. More means more memory use,
//but less overhead for setting up / finishing transfers. Make sure 240 is dividable by this.
#define app_disp_PARALLEL_LINES_D 	(1)	//TODO VO If set to any higher than 1, each line is shifted by 1 in comparison with previous line, which creates distorted image

#define DISP_ROWS_D					(120)
#define DISP_COLUMNS_D				(120)

#define COLOUR_WHITE_D				(0xFFFF)
#define COLOUR_BLACK_D				(0x0000)
#define COLOUR_RED_D				(0x03E0)
#define COLOUR_GREEN_D				(0x001F)
#define COLOUR_BLUE_D				(0xFC00)

#define INPUT_ARRAY_LENGTH_D    (24)
#define INPUT_ARRAY_WIDTH_D     (32)
#define SCALING_FACTOR_D        (1)
#define OUTPUT_ARRAY_LENGTH_D   ((INPUT_ARRAY_LENGTH_D * SCALING_FACTOR_D) - SCALING_FACTOR_D)
#define OUTPUT_ARRAY_WIDTH_D    ((INPUT_ARRAY_WIDTH_D * SCALING_FACTOR_D) - SCALING_FACTOR_D)
#define OUTPUT_NUM_OF_PIXELS_D  (OUTPUT_ARRAY_LENGTH_D * OUTPUT_ARRAY_WIDTH_D)
// Output buffer size shall be multiple of OUTPUT_ARRAY_WIDTH_D
#define OUTPUT_BUFFER_SIZE_D    (32 * OUTPUT_ARRAY_LENGTH_D)

/************************************************************************************/
/* TYPEDEFS																			*/
/************************************************************************************/

/*
 The LCD needs a bunch of command/argument values to be initialized. They are stored in this struct.
*/
typedef struct
{
    uint8_t cmd;
    uint8_t data[16];
    uint8_t databytes; //No of data in data; bit 7 = delay after set; 0xFF = end of cmds.
}
lcd_init_cmd_t;

typedef enum
{
    LCD_TYPE_ILI = 1,
    LCD_TYPE_ST,
    LCD_TYPE_MAX,
}
type_lcd_t;

//Data that is passed from the decoder function to the infunc/outfunc functions.
typedef struct
{
    const unsigned char *inData;	//Pointer to jpeg data
    int inPos;						//Current position in jpeg data
    uint16_t **outData;				//Array of IMAGE_H pointers to arrays of IMAGE_W 16-bit pixel values
    int outW;						//Width of the resulting file
    int outH;						//Height of the resulting file
}
JpegDev;

typedef enum
{
	colour_White_e 	= 0xFFFF,
	colour_Red_e	= 0x03E0,
	colour_Green_e	= 0x001F,
	colour_Blue_e	= 0xFC00,
	colour_Black_e	= 0x0000
}
COLOURS_T;

typedef struct
{
	uint16_t	*pu16DispPixels;
	uint16_t 	u16Status;
}
DISP_HANDLE_T;


/************************************************************************************/
/* FUNCTION PROTOTYPES																*/
/************************************************************************************/

void app_disp_vInitialize(void);

uint32_t app_disp_u32LCD_GetID(void);

void app_disp_vLCD_SPI_PreTransferCallback(spi_transaction_t *psTransaction);

void app_disp_vRunDisplayTask(void);

void app_disp_vSetRectangleColour(uint16_t u16ColPos, uint16_t u16RowPos, uint16_t u16ColLength, uint16_t u16RowLength, uint16_t u16Colour);

void app_disp_vGetFreeHandle(DISP_HANDLE_T* psHandle);

#if ENABLE_DISP_FRAME_RATE_MEASURE_D
	uint16_t app_disp_u16GetFrameRate(void);
#endif

void LCD_Convert_u16(uint16_t *frameTemperature, uint16_t *frameColor, uint16_t frameSize, uint16_t minTemperature, uint16_t maxTemperature);

#endif /* APP_DISPLAY_H_ */
