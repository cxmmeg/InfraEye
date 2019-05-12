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

void vTask1( void *pvParameters )
{
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 1000;

	xLastWakeTime = xTaskGetTickCount();
	for(;;)
	{
		vTaskDelayUntil( &xLastWakeTime, xFrequency );
		ets_printf("Task1\n");
	}
	vTaskDelete(NULL);
}

void vTask2( void *pvParameters )
{
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 100;
	uint16_t u16RectPosCol = 0u;
	uint16_t u16RectPosRow = 0u;
	uint16_t u16LastRectPosCol = 216u;
	uint16_t u16LastRectPosRow = 288u;

	app_disp_vSetRectangleColour(0u,0u,240u,320u,(uint16_t)colour_Red_e);
	xLastWakeTime = xTaskGetTickCount();
	for(;;)
	{
		vTaskDelayUntil( &xLastWakeTime, xFrequency );
		ets_printf("Task2\n");
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
	vTaskDelete(NULL);
}

void app_main()
{
    uint32_t u32TimeStamp = 0uL;

	TaskHandle_t xHandleTask1 = NULL;
	TaskHandle_t xHandleTask2 = NULL;

    app_disp_vInitialize();

	xTaskCreate(vTask1, "Task1",
	                    1500,      /* Stack size in words, not bytes. */
	                    ( void * ) 1,    /* Parameter passed into the task. */
	                    1,
	                    &xHandleTask1 );      /* Used to pass out the created task's handle. */

	xTaskCreate(vTask2, "Task2",
	                    1500,      /* Stack size in words, not bytes. */
	                    ( void * ) 1,    /* Parameter passed into the task. */
	                    2,
	                    &xHandleTask2 );      /* Used to pass out the created task's handle. */

    while(1)
    {
        u32TimeStamp = (uint32_t)xTaskGetTickCount();

        /* Run display task */
    	app_disp_vRunDisplayTask();
    }
}
