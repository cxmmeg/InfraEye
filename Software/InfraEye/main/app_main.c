/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

/************************************************************************************/
/* INCLUDES																			*/
/************************************************************************************/

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#include "app_display.h"

/************************************************************************************/
/* DEFINES																			*/
/************************************************************************************/

/* Can run 'make menuconfig' to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define BLINK_GPIO 2

/************************************************************************************/
/* FUNCTION PROTOTYPES																*/
/************************************************************************************/

/************************************************************************************/
/* FUNCTION DEFINITIONS																*/
/************************************************************************************/
void app_main()
{
    /* Configure the IOMUX register for pad BLINK_GPIO (some pads are
       muxed to GPIO on reset already, but some default to other
       functions and need to be switched to GPIO. Consult the
       Technical Reference for a list of pads and their default
       functions.)
    */
#if 0
    gpio_pad_select_gpio(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
#endif

    esp_err_t ret;
	spi_device_handle_t spi;
	spi_bus_config_t buscfg =
	{
		.miso_io_num=PIN_NUM_MISO,
		.mosi_io_num=PIN_NUM_MOSI,
		.sclk_io_num=PIN_NUM_CLK,
		.quadwp_io_num=-1,
		.quadhd_io_num=-1,
		.max_transfer_sz=PARALLEL_LINES*320*2+8
	};
	spi_device_interface_config_t devcfg =
	{
    #ifdef CONFIG_LCD_OVERCLOCK
		.clock_speed_hz=26*1000*1000,           //Clock out at 26 MHz
    #else
		.clock_speed_hz=10*1000*1000,           //Clock out at 10 MHz
    #endif
		.mode=0,                                //SPI mode 0
		.spics_io_num=PIN_NUM_CS,               //CS pin
		.queue_size=7,                          //We want to be able to queue 7 transactions at a time
		.pre_cb=lcd_spi_pre_transfer_callback,  //Specify pre-transfer callback to handle D/C line
	};
	//Initialize the SPI bus
	ret = spi_bus_initialize(HSPI_HOST, &buscfg, 1);
	ESP_ERROR_CHECK(ret);
	//Attach the LCD to the SPI bus
	ret = spi_bus_add_device(HSPI_HOST, &devcfg, &spi);
	ESP_ERROR_CHECK(ret);
	//Initialize the LCD
	lcd_init(spi);
	//Initialize the effect displayed
	ret = pretty_effect_init();
	ESP_ERROR_CHECK(ret);

	//Go do nice stuff.
	display_pretty_colors(spi);

#if 0
    while(1)
    {
        /* Blink off (output low) */
    	printf("Turning off the LED\n");
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        /* Blink on (output high) */
        printf("Turning on the LED\n");
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
#endif
}
