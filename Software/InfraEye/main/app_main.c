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
#include "module_TouchScreen.h"

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
uint16_t u16_pixelValue[768];

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
	uint8_t subPage;

	xLastWakeTime = xTaskGetTickCount();
	for(;;)
	{
		subPage = IRsensor_CurrentSubPage();
		// -------------- Read subframe ----------------------------
		//IRsensor_LoadSubPage(pixelValue);
		IRsensor_LoadSubPage_u16(u16_pixelValue);
		vTaskDelayUntil( &xLastWakeTime, xFrequency );
		ets_printf("FPS:%d\n", app_disp_u16GetFrameRate());
	}
	vTaskDelete(NULL);
}

void vTask2( void *pvParameters )
{
	app_disp_vSetRectangleColour(0u,0u,DISP_ROWS_D, DISP_COLUMNS_D, (uint16_t)colour_Green_e);
	app_disp_vSetRectangleColour(10u,10u,100,100u,(uint16_t)colour_Blue_e);
	app_disp_vSetRectangleColour(20u,20u,80u,80u,(uint16_t)colour_Red_e);
	app_disp_vSetRectangleColour(30u,30u,60u,60u,(uint16_t)colour_White_e);
	app_disp_vSetRectangleColour(40u,40u,40u,40u,(uint16_t)colour_Black_e);

	//xLastWakeTime = xTaskGetTickCount();
	for(;;)
	{
		for(;;)
		{

		}

	}
	vTaskDelete(NULL);
}

void app_main()
{
	TaskHandle_t xHandleTask1 = NULL;
	TaskHandle_t xHandleTask2 = NULL;
	uint8_t address = 0;

	Touch_Init();
    app_disp_vInitialize();
    address = IRsensor_Init();
    if(address==0xFF)
    {
    	u16Color = colour_Red_e;
    }
    else
    {
    	u16Color = colour_Green_e;
    }
	ets_printf("Address:0x%x\n", address);

	xTaskCreate(vTask1, "IRsens",
	                    1500,      /* Stack size in words, not bytes. */
	                    ( void * ) 1,    /* Parameter passed into the task. */
	                    1,
	                    &xHandleTask1 );      /* Used to pass out the created task's handle. */

	xTaskCreate(vTask2, "LCD",
	                    1500,      /* Stack size in words, not bytes. */
	                    ( void * ) 1,    /* Parameter passed into the task. */
	                    2,
	                    &xHandleTask2 );      /* Used to pass out the created task's handle. */

    while(1)
    {
      	app_disp_vRunDisplayTask();
    }
}
