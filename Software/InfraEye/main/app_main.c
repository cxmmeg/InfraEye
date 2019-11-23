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
#include "freertos/timers.h"
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

#define TASK_IR_SENSOR_CYCLE_RATE_MS ((TickType_t)8)
#define TASK_PERFORM_IR_DATA_CYCLE_RATE_MS ((TickType_t)100)

uint32_t u32FrameRate = 0uL;	// Number or Idle task executions in 1 second
uint32_t u32FrameRateOld = 0uL;	// Number or Idle task executions in 1 second
uint16_t u16Color;
uint16_t app_main_au16PixelValue[768] = {0u};
uint16_t app_main_au16DisplayFrame[768] = {0u};

uint16_t app_main_u16MaxTemp;
uint16_t app_main_u16MinTemp;

TaskHandle_t xHandleTask1 = NULL;
TaskHandle_t xHandleTask2 = NULL;

TimerHandle_t xTimer_1;

uint8_t app_main_u8DataReady = 0u;

/************************************************************************************/
/* FUNCTION PROTOTYPES																*/
/************************************************************************************/

void vTimerCallback( TimerHandle_t xTimer );

/************************************************************************************/
/* FUNCTION DEFINITIONS																*/
/************************************************************************************/

void vTask1( void *pvParameters )
{
	TickType_t xLastWakeTime;

	xLastWakeTime = xTaskGetTickCount();
	for(;;)
	{
		/* Task resumed from timer callback */
		/* Read measured data from IR sensor */
		IRsensor_vLoadSubPage_u16();

		/* Calculate pixel temperatures */
		IRsensor_vCalculatePixelTemp_u16(app_main_au16PixelValue);

		/* Both subpages are measured */
		if(IRsensor_u8CurrentSubPage())
		{
			/* Update min/max temperature values */
			IRsensor_UpdateMinMax_u16(&app_main_u16MinTemp, &app_main_u16MaxTemp, app_main_au16PixelValue);

			//LCD_Convert_u16(app_main_au16PixelValue, app_main_au16DisplayFrame, 768, app_main_u16MinTemp, app_main_u16MaxTemp);

			app_main_u8DataReady = 1u;
			ets_printf("TempMin = %d, TempMax = %d\n",(int16_t)((app_main_u16MinTemp >> TEMP_SCALE_BIT_SHIFT_D) - TEMP_OFFSET_D),
														(int16_t)(app_main_u16MaxTemp >> TEMP_SCALE_BIT_SHIFT_D) - TEMP_OFFSET_D);

#if 0
			for(uint16_t i = 0u; i < 768u; i++)
			{
				if(i % 32 == 0)
				{
					ets_printf("\n");
				}
				ets_printf("%d,",app_main_au16PixelValue[i]);
			}
#endif
		}
		/* Task is finished - start timer again */
		xTimerStart(xTimer_1, 0);
		/* Suspend the task */
		vTaskSuspend(NULL);
	}

	vTaskDelete(NULL);
}

void vTask2( void *pvParameters )
{
	TickType_t xLastWakeTime;
	uint16_t u16DispCoordinateX, u16DispCoordinateY;

	xLastWakeTime = xTaskGetTickCount();
	for(;;)
	{
		vTaskDelayUntil( &xLastWakeTime, 128);
	    gpio_set_level(26, 1);
		if(app_main_u8DataReady)
		{
#if 0
			for (uint16_t u16Iterator = 0; u16Iterator < 10; u16Iterator++)
			{
				u16DispCoordinateX = 31 - (u16Iterator / 23);
				u16DispCoordinateY = u16Iterator % 23;

				//app_disp_vSetRectangleColour(4*u16DispCoordinateX,4*u16DispCoordinateY,4u,4u,app_main_au16DisplayFrame[u16Iterator]);
			}
#endif
			app_main_u8DataReady = 0u;
		}
		gpio_set_level(26, 0);


	}

	vTaskDelete(NULL);
}

void vTimerCallback( TimerHandle_t xTimer )
{
	/* Check if new measured data is ready */
	if(IRsensor_u8DataIsReady())
	{
		/* Data is ready - stop timer and resume data acquire task */
		xTimerStop(xTimer_1, 0);
		vTaskResume(xHandleTask1);
	}
	else
	{
		/* Do nothing */
	}

}

void app_main()
{

	/* Configure PIN 27 as output for IR sensor power supply */
	gpio_pad_select_gpio(27);
	gpio_set_direction(27, GPIO_MODE_OUTPUT);

	/* Configure PIN 26 as output for debug purposes */
	gpio_pad_select_gpio(26);
	gpio_set_direction(26, GPIO_MODE_OUTPUT);
    gpio_set_level(26, 0);

	/* Power reset IR sensor */
    gpio_set_level(27, 0);
    vTaskDelay(200);
    gpio_set_level(27, 1);
    vTaskDelay(200);

    /* Initialize display */
    app_disp_vInitialize();

    /* Initialize IR sensor */
    (void)IRsensor_Init();

    /* Set dummy pattern for display */
	app_disp_vSetRectangleColour(0u,0u,DISP_ROWS_D, DISP_COLUMNS_D, (uint16_t)colour_Green_e);
	app_disp_vSetRectangleColour(10u,10u,100,100u,(uint16_t)colour_Blue_e);
	app_disp_vSetRectangleColour(20u,20u,80u,80u,(uint16_t)colour_Red_e);
	app_disp_vSetRectangleColour(30u,30u,60u,60u,(uint16_t)colour_White_e);
	app_disp_vSetRectangleColour(40u,40u,40u,40u,(uint16_t)colour_Black_e);

	/* Create timer for check if data is ready */
	xTimer_1 = xTimerCreate ("Timer",pdMS_TO_TICKS( 4 ),pdTRUE,( void * ) 0,vTimerCallback);


	/* Create task for measured data acquisition from IR sensor */
	xTaskCreate(vTask1, "IRsens",
	                    1500,      /* Stack size in words, not bytes. */
	                    ( void * ) 1,    /* Parameter passed into the task. */
	                    2,
	                    &xHandleTask1 );      /* Used to pass out the created task's handle. */

	/* Create task for displaying measured data */
	xTaskCreate(vTask2, "LCD",
	                    1500,      /* Stack size in words, not bytes. */
	                    ( void * ) 1,    /* Parameter passed into the task. */
	                    1,
	                    &xHandleTask2 );      /* Used to pass out the created task's handle. */

	/* Suspend tasks */
	vTaskSuspend(xHandleTask1);
	vTaskSuspend(xHandleTask2);

	/* Start timer */
	if(xTimer_1 != NULL)
	{
		 xTimerStart(xTimer_1, 0);
	}

    while(1)
    {
      	app_disp_vRunDisplayTask();
    }
}
