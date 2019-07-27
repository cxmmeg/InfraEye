/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

/************************************************************************************/
/* INCLUDES																			*/
/************************************************************************************/

#include "module_IRsensor.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "soc/gpio_struct.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "sdkconfig.h"

#include "app_display.h"

/************************************************************************************/
/* DEFINES																			*/
/************************************************************************************/

/* Can run 'make menuconfig' to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define BLINK_GPIO 2

uint32_t u32FrameRate = 0uL;	// Number or Idle task executions in 1 second
uint32_t u32FrameRateOld = 0uL;	// Number or Idle task executions in 1 second
uint16_t u16Color;

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
		ets_printf("FPS:%d\n", u32FrameRateOld);
	}
	vTaskDelete(NULL);
}

void vTask2( void *pvParameters )
{
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 14;
	uint16_t u16RectPosCol = 0u;
	uint16_t u16RectPosRow = 0u;
	uint16_t u16LastRectPosCol = 216u;
	uint16_t u16LastRectPosRow = 288u;

	app_disp_vSetRectangleColour(0u,0u,240u,320u,(uint16_t)colour_Green_e);
	xLastWakeTime = xTaskGetTickCount();
	for(;;)
	{
		vTaskDelayUntil( &xLastWakeTime, xFrequency );
		//ets_printf("Task2\n");
    	/* Set last position to blue colour */
        app_disp_vSetRectangleColour(u16LastRectPosCol,u16LastRectPosRow,24u,32u,(uint16_t)colour_Blue_e);
        /* Set new rectangle position */
        app_disp_vSetRectangleColour(u16RectPosCol,u16RectPosRow,24u,32u,(uint16_t)colour_Red_e);

        u16LastRectPosCol = u16RectPosCol;
        u16LastRectPosRow = u16RectPosRow;

        /* Move rectangle */
    	if((u16RectPosRow + 32u) < 320u)
    	{
    		u16RectPosRow += 1u ;
    	}
    	else
    	{
    		u16RectPosRow = 0u;

    		if((u16RectPosCol + 24u) < 240u)
    		{
    			u16RectPosCol += 6u;
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
	uint32_t u32TimeIdleStart = 0uL;
	uint32_t u32TimeIdleEnd = 0uL;

	TaskHandle_t xHandleTask1 = NULL;
	TaskHandle_t xHandleTask2 = NULL;

	uint8_t address = 0;

    app_disp_vInitialize();
    address = IRsensor_Init();
    if(address==0xFF)
    {
    	u16Color = colour_White_e;
    }
    else
    {
    	u16Color = colour_Red_e;
    }
	ets_printf("Address:0x%x\n", address);

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
	u32TimeIdleStart = (uint32_t)xTaskGetTickCount();
    while(1)
    {
      	app_disp_vRunDisplayTask();
    	u32TimeIdleEnd = (uint32_t)xTaskGetTickCount();
    	if((u32TimeIdleEnd - u32TimeIdleStart) > 1000)
    	{
    		u32TimeIdleStart = (uint32_t)xTaskGetTickCount();
    		u32FrameRateOld = u32FrameRate;
    		u32FrameRate = 0;
    	}else
    	{
    		u32FrameRate++;
    	}
    }
}
