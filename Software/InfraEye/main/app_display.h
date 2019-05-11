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

//To speed up transfers, every SPI transfer sends a bunch of lines. This define specifies how many. More means more memory use,
//but less overhead for setting up / finishing transfers. Make sure 240 is dividable by this.
#define app_disp_PARALLEL_LINES_D 	(48)

//Define the height and width of the jpeg file. Make sure this matches the actual jpeg
//dimensions.
#define IMAGE_W 336
#define IMAGE_H 256

//Size of the work space for the jpeg decoder.
#define WORKSZ 3100

#define DISP_ROWS_D					(320)
#define DISP_COLUMNS_D				(240)

#define COLOUR_WHITE_D				(0xFFFF)
#define COLOUR_BLACK_D				(0x0000)
#define COLOUR_RED_D				(0xF800)
#define COLOUR_GREEN_D				(0x07E0)
#define COLOUR_BLUE_D				(0x001F)



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
	colour_Red_e	= 0xF800,
	colour_Green_e	= 0x07E0,
	colour_Blue_e	= 0x001F,
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

/**
 * @brief Calculate the effect for a bunch of lines.
 *
 * @param dest Destination for the pixels. Assumed to be LINECT * 320 16-bit pixel values.
 * @param line Starting line of the chunk of lines.
 * @param frame Current frame, used for animation
 * @param linect Amount of lines to calculate
 */
//void pretty_effect_calc_lines(uint16_t *dest, int line, int frame, int linect);


/**
 * @brief Initialize the effect
 *
 * @return ESP_OK on success, an error from the jpeg decoder otherwise.
 */
//esp_err_t pretty_effect_init();


/**
 * @brief Decode the jpeg ``image.jpg`` embedded into the program file into pixel data.
 *
 * @param pixels A pointer to a pointer for an array of rows, which themselves are an array of pixels.
 *        Effectively, you can get the pixel data by doing ``decode_image(&myPixels); pixelval=myPixels[ypos][xpos];``
 * @return - ESP_ERR_NOT_SUPPORTED if image is malformed or a progressive jpeg file
 *         - ESP_ERR_NO_MEM if out of memory
 *         - ESP_OK on succesful decode
 */
esp_err_t decode_image(uint16_t ***pixels);

void app_disp_vInitialize(void);

uint32_t app_disp_u32LCD_GetID(void);

void app_disp_vLCD_SPI_PreTransferCallback(spi_transaction_t *psTransaction);

void app_disp_vRunDisplayTask(void);

void app_disp_vSetRectangleColour(uint16_t u16ColPos, uint16_t u16RowPos, uint16_t u16ColLength, uint16_t u16RowLength, uint16_t u16Colour);

void app_disp_vGetFreeHandle(DISP_HANDLE_T* psHandle);

//void display_pretty_colors(void);


#endif /* APP_DISPLAY_H_ */
