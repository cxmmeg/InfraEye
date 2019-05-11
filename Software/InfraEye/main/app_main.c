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

	uint32_t u32TimeStamp = 0uL;

	uint16_t u16RectPosCol = 0u;
	uint16_t u16RectPosRow = 0u;
	uint16_t u16LastRectPosCol = 216u;
	uint16_t u16LastRectPosRow = 288u;



    gpio_pad_select_gpio(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

    app_disp_vInitialize();

    app_disp_vSetRectangleColour(0u,0u,240u,320u,(uint16_t)colour_Blue_e);
    while(1)
    {
        u32TimeStamp = (uint32_t)xTaskGetTickCount();

        if(u32TimeStamp % 50 == 0uL)
        {
        	/* Set last position to blue colour */
            app_disp_vSetRectangleColour(u16LastRectPosCol,u16LastRectPosRow,24u,32u,(uint16_t)colour_Blue_e);
            /* Set new rectangle position */
            app_disp_vSetRectangleColour(u16RectPosCol,u16RectPosRow,24u,32u,(uint16_t)colour_White_e);

            u16LastRectPosCol = u16RectPosCol;
            u16LastRectPosRow = u16RectPosRow;

            /* Move rectangle */
        	if((u16RectPosRow + 32u) < 320u)
        	{
        		u16RectPosRow += 16u ;
        	}
        	else
        	{
        		u16RectPosRow = 0u;

        		if((u16RectPosCol + 24u) < 240u)
        		{
        			u16RectPosCol += 12u;
        		}
        		else
        		{
        			u16RectPosCol = 0u;
        		}
        	}
        }

        /* Run display task */
    	app_disp_vRunDisplayTask();

    }
}
