/*
 * rtos_driver.c
 *
 *  Created on: 03/04/2019
 *      Author: HEMI
 */

#include "rtos_driver.h"

/*********************************************************************************************/

typedef struct {
	uint8_t init_val;
	SemaphoreHandle_t sem_rx_binary;
	SemaphoreHandle_t mutex;
	EventGroupHandle_t event_group;
	QueueHandle_t queue;
}RTOS_CAN_Handler_t;

/* The number of items the queue can hold.  This is 1 as the receive task
will remove items as they are added, meaning the send task should always find
the queue empty. */
#define mainQUEUE_LENGTH					( 5 )
/* The LED will remain on until the button has not been pushed for a full
5000ms. */
#define mainBUTTON_LED_TIMER_PERIOD_MS		( 5000UL / portTICK_PERIOD_MS )
#define IS_INIT								( 1 )
#define NOT_INIT							( 0 )
#define EVENT_GROUP_ADC						( 1 )
#define EVENT_GROUP_SW						( 2 )



/*********************************************************************************************/

static TimerHandle_t xButtonLEDTimer = NULL;
static RTOS_CAN_Handler_t can_handler = { 0 };

static uint32_t task_period = 100U;
static uint16_t ADC_value_received = 0;
static uint16_t ADC_threshold_LED = 1500;

/*********************************************************************************************/

void rtos_start(CAN_Type* base, uint32_t speed)
{
	can_handler.init_val = IS_INIT;
	can_handler.sem_rx_binary = xSemaphoreCreateBinary();
	can_handler.mutex = xSemaphoreCreateMutex();
	can_handler.event_group = xEventGroupCreate();
	can_handler.queue = xQueueCreate( mainQUEUE_LENGTH, sizeof( uint16_t ) * 5 );

	/*********************** NOTE ***************************/
	/** This module is taken from the driver example FlexCAN*/
	/********************************************************/
	/** From here **********************************************************************************************/
	SOSC_init_8MHz();       /* Initialize system oscillator for 8 MHz xtal */
	SPLL_init_160MHz();     /* Initialize SPLL to 160 MHz with 8 MHz SOSC */
	NormalRUNmode_80MHz();  /* Init clocks: 80 MHz sysclk & core, 40 MHz bus, 20 MHz flash */
	/** To here **********************************************************************************************/

	CAN_Init(base, speed);

	/*********************** NOTE ***************************/
	/** This module is taken from the driver example FlexCAN*/
	/********************************************************/
	/** From here **********************************************************************************************/
	PORT_init();             /* Configure ports */
	LPSPI1_init_master();    /* Initialize LPSPI1 for communication with MC33903 */
	LPSPI1_init_MC33903();   /* Configure SBC via SPI for CAN transceiver operation */
	/** To here **********************************************************************************************/

	//xTaskCreate( prvQueueReceiveTask, "RX", configMINIMAL_STACK_SIZE, NULL, mainQUEUE_RECEIVE_TASK_PRIORITY, NULL );
	//xTaskCreate( prvQueueSendTask, "TX", configMINIMAL_STACK_SIZE, NULL, mainQUEUE_SEND_TASK_PRIORITY, NULL );


	/* Start the tasks and timer running. */
	vTaskStartScheduler();

}

void rtos_can_tx_thread(void* args)
{
	if (IS_INIT == can_handler.init_val)
	{
		EventBits_t tx_event;

		for(;;)
		{
			xEventGroupWaitBits(can_handler.event_group, EVENT_GROUP_ADC | EVENT_GROUP_SW, pdFALSE, pdFALSE, portMAX_DELAY);
			tx_event = xEventGroupGetBits(can_handler.event_group);
			xEventGroupClearBits(can_handler.event_group, tx_event);

			if(EVENT_GROUP_ADC == tx_event)
			{
				//TODO: Send ADC

			}

			else if(EVENT_GROUP_SW == tx_event)
			{
				//TODO: Set SW action

			}
		}
	}
}

void rtos_can_rx_thread_periodic(void *args)
{


	if (IS_INIT == can_handler.init_val)
	{
		for(;;)
		{
			//TODO: Set Rx periodic action

			vTaskDelay(pdMS_TO_TICKS(task_period));
		}
	}
}

void rtos_can_rx_thread_interrupt(void *args)
{


	if (IS_INIT == can_handler.init_val)
	{
		for(;;)
		{
			xSemaphoreTake(can_handler.sem_rx_binary, portMAX_DELAY);
		}
	}
}

void rtos_can_LED_control(void *args)
{
	if(IS_INIT == can_handler.init_val)
	{


		for(;;)
		{
			if(ADC_threshold_LED < ADC_value_received)
			{
				//TODO: Turn on LED

			}

			vTaskDelay(pdMS_TO_TICKS(task_period));
		}
	}
}

/* The ISR executed when the user button is pushed. */
//void vPort_C_ISRHandler( void )
//{
    //portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	/* The button was pushed, so ensure the LED is on before resetting the
	LED timer.  The LED timer will turn the LED off if the button is not
	pushed within 5000ms. */
    //GPIO_HAL_ClearPins(LED_GPIO, (1 << LED1));
	/* This interrupt safe FreeRTOS function can be called from this interrupt
	because the interrupt priority is below the
	configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY setting in FreeRTOSConfig.h. */
	//xTimerResetFromISR( xButtonLEDTimer, &xHigherPriorityTaskWoken );

	/* Clear the interrupt before leaving. */
	//PORT_HAL_ClearPortIntFlagCmd(BTN_PORT);

	/* If calling xTimerResetFromISR() caused a task (in this case the timer
	service/daemon task) to unblock, and the unblocked task has a priority
	higher than or equal to the task that was interrupted, then
	xHigherPriorityTaskWoken will now be set to pdTRUE, and calling
	portEND_SWITCHING_ISR() will ensure the unblocked task runs next. */
	//portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
//}

//static void prvButtonLEDTimerCallback( TimerHandle_t xTimer )
//{
	/* Casting xTimer to void because it is unused */
	//(void)xTimer;

	/* The timer has expired - so no button pushes have occurred in the last
	five seconds - turn the LED off. */
	//GPIO_HAL_SetPins(LED_GPIO, (1 << LED1));
//}

/*-----------------------------------------------------------*/

