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
#include "module_TouchScreen.h"

/************************************************************************************/
/* DEFINES																			*/
/************************************************************************************/

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
	ets_printf("TASK1/IR sensor task: Initial start\n");
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

			LCD_Convert_u16(app_main_au16PixelValue, app_main_au16DisplayFrame, 768, app_main_u16MinTemp, app_main_u16MaxTemp);

			app_main_u8DataReady = 1u;
//			ets_printf("TempMin = %d, TempMax = %d\n",(int16_t)((app_main_u16MinTemp >> TEMP_SCALE_BIT_SHIFT_D) - TEMP_OFFSET_D),
	//													(int16_t)(app_main_u16MaxTemp >> TEMP_SCALE_BIT_SHIFT_D) - TEMP_OFFSET_D);

#if 0
			for(uint16_t i = 0u; i < 768u; i++)
			{
				if(i % 32 == 0)
				{
					ets_printf("\n");
				}
				ets_printf("%d,",(app_main_au16PixelValue[i] >> TEMP_SCALE_BIT_SHIFT_D) - TEMP_OFFSET_D);
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
	uint8_t multiply = 3u;
	uint32_t X = 0;
	uint8_t count = 0;

	xLastWakeTime = xTaskGetTickCount();
	ets_printf("TASK2/LCD task: Initial start\n");
	for(;;)
	{
		vTaskDelayUntil( &xLastWakeTime, 128);
//	    gpio_set_level(26, 1);
//	    ets_printf("A\n");
		if(app_main_u8DataReady)
		{
			//ets_printf("B\n");
#if 1
			for (uint16_t u16Iterator = 0; u16Iterator < 768; u16Iterator++)
			{
				//ets_printf("C\n");
				u16DispCoordinateX = u16Iterator % 32;
				u16DispCoordinateY = u16Iterator / 32;

				app_disp_vSetRectangleColour(multiply*u16DispCoordinateX, multiply*u16DispCoordinateY, multiply, multiply, app_main_au16DisplayFrame[u16Iterator]);
				//app_disp_vSetPixelColour(u16DispCoordinateX, u16DispCoordinateY, app_main_au16DisplayFrame[u16Iterator]);
			}
#endif
			app_main_u8DataReady = 0u;
		}
//		gpio_set_level(26, 0);
#if 0
		if(count == 10)
		{
			ets_printf("Read X\n");
			X = Touch_GetSome();
			ets_printf("X=0x%4.x\n", X);
			count = 0;
		}else
		{
			count++;
		}
#endif
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
	uint32_t u32Return = 0;
#if 0
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
#endif
    /* Initialize display */
    app_disp_vInitialize();
//    ets_printf("Init touch sensor\n");
//    u32Return = Touch_Init();
//    ets_printf("Err code: 0x%4.x\n", u32Return);
    /* Initialize IR sensor */
    (void)IRsensor_Init();

    /* Set dummy pattern for display */
    app_disp_vSetRectangleColour(0u,0u,DISP_ROWS_D, DISP_COLUMNS_D, (uint16_t)colour_Black_e);
//	app_disp_vSetRectangleColour(0u,0u,DISP_ROWS_D, DISP_COLUMNS_D, (uint16_t)colour_Red_e);
//	app_disp_vSetRectangleColour(10u,10u,100,100u,(uint16_t)colour_Green_e);
//	app_disp_vSetRectangleColour(20u,20u,80u,80u,(uint16_t)colour_Blue_e);
//	app_disp_vSetRectangleColour(30u,30u,60u,60u,(uint16_t)colour_White_e);
//	app_disp_vSetRectangleColour(40u,40u,40u,40u,(uint16_t)colour_Black_e);

	uint16_t color[120], source[120];
	uint8_t Y;
	for(Y=0;Y<120;Y++)
	{
		source[Y] = Y;
	}

	LCD_Convert_u16(source, color, 120, 0, 120);

	for(Y=0;Y<120;Y++)
	{
		app_disp_vSetRectangleColour(120-Y, 110, 1u, 10u, color[Y]);
	}

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
//	vTaskSuspend(xHandleTask2);

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
