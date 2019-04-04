

/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"

/* SDK includes. */
#include "gpio_hal.h"
#include "port_hal.h"
#include "pcc_hal.h"
#include "interrupt_manager.h"
#include "clock_manager.h"
#include "clockMan1.h"
#include "pin_mux.h"

#include "BoardDefines.h"
#include "rtos_can_driver.h"


/* Priorities at which the tasks are created. */
#define mainQUEUE_RECEIVE_TASK_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define	mainQUEUE_SEND_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )
/* The rate at which data is sent to the queue, specified in milliseconds, and
converted to ticks using the portTICK_PERIOD_MS constant. */
#define mainQUEUE_SEND_FREQUENCY_MS			( 200 / portTICK_PERIOD_MS )
/* The LED will remain on until the button has not been pushed for a full
5000ms. */
#define mainBUTTON_LED_TIMER_PERIOD_MS		( 5000UL / portTICK_PERIOD_MS )
/* The number of items the queue can hold.  This is 1 as the receive task
will remove items as they are added, meaning the send task should always find
the queue empty. */
#define mainQUEUE_LENGTH					( 1 )
/* The LED toggle by the queue receive task (blue). */
#define mainTASK_CONTROLLED_LED				( 1UL << 0UL )
/* The LED turned on by the button interrupt, and turned off by the LED timer
(green). */
#define mainTIMER_CONTROLLED_LED			( 1UL << 1UL )
/* The vector used by the GPIO port C.  Button SW7 is configured to generate
an interrupt on this port. */
#define mainGPIO_C_VECTOR					( 61 )
/* A block time of zero simply means "don't block". */
#define mainDONT_BLOCK						( 0UL )

/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/

/** Defines the initial value for the variables*/
#define INIT_VAL				(0)
/** Defines the size of the RAM for the message buffers*/
#define MAX_MSG_BUFFERS			(128)
/** Defines the number of ID filters*/
#define MAX_FILTER_BUFFERS		(16)

/** Defines the length of a buffer*/
#define MSG_BUF_SIZE			(4)

/** Defines the RX mask to enable the buffer*/
#define ENABLE_RX_BUFF			(0x04000000)

/** Defines the value to accept all IDs*/
#define NOT_CHECK_ANY_ID		(0x00000000)

/** Defines the bits to clear the interruption flag of the Rx MB*/
#define CLEAR_MB_0				(0x00000001)
/** Defines the mask for the standard ID*/
#define STD_ID_MASK				(0x000007FF)
/** Defines the shifts for the standard ID*/
#define STD_ID_SHIFT				(18)

/** Defines the transmit code*/
#define TX_BUFF_TRANSMITT		(0x0C400000)

/** Defines the mask for the MB code*/
#define CAN_CODE_MASK			(0x07000000)
/** Defines the shift for the MB code*/
#define CAN_CODE_SHIFT			(24)

/** Defines the mask for the time stamp*/
#define CAN_TIMESTAMP_MASK		(0x0000FFFF)
/** Defines the bits to clear the interruption flag of the Tx MB*/
#define CLEAR_MB_4				(0x00000010)

/** Defines the mask for the LSB*/
#define BIT_MASK				(1)
/** Defines the bits to clear al MB interruption flags*/
#define CLEAR_ALL_FLAGS			(0xFFFFFFFF)

/** Defines the Rx MB offset in RAM array*/
#define RX_BUFF_OFFSET			(0x04)
/** Defines the Tx MB offset in RAM array*/
#define TX_BUFF_OFFSET			(0x00)
/** Defines the code and DLC position in the MB array*/
#define CODE_AND_DLC_POS		(0x00)
/** Defines the ID position in the MB array*/
#define ID_POS					(0x01)
/** Defines the message start position in the MB array*/
#define MSG_POS					(0x02)
/** Defines the max data size of the MB array*/
#define DATA_SIZE				(0x02)

/** Defines the divisor to convert from DLC to the msg size*/
#define DLC_TO_MSG_SIZE_DIV		(0x04)
/** Defines the shifts for the Rx MB interruption flag*/
#define RX_MB_FLAG_SHIFT		(0x04)

/** Disable CAN FS*/
#define CAN_FD_DISABLE			(0x0000001F)
/** Offset to calculate the msg_size from DLC*/
#define MESSAGE_SIZE_OFF		(0x03)
/** Delay for the Tx*/
#define CAN_DELAY					(10000)

/*********************************************************************************************/
/*********************************************************************************************/
/*********************************************************************************************/

/*
 * Setup the NVIC, LED outputs, and button inputs.
 */
static void prvSetupHardware( void );

/*
 * The tasks as described in the comments at the top of this file.
 */
static void prvQueueReceiveTask( void *pvParameters );
static void prvQueueSendTask( void *pvParameters );

/*
 * The LED timer callback function.  This does nothing but switch off the
 * LED defined by the mainTIMER_CONTROLLED_LED constant.
 */
static void prvButtonLEDTimerCallback( TimerHandle_t xTimer );

/*-----------------------------------------------------------*/

/* The queue used by both tasks. */
static QueueHandle_t xQueue = NULL;

/* The LED software timer.  This uses prvButtonLEDTimerCallback() as its callback
function. */
static TimerHandle_t xButtonLEDTimer = NULL;

/*-----------------------------------------------------------*/


/**************************************************************************************************/
/** This function initializes the CAN*/
void CAN_Init(CAN_Type* base, uint32_t speed)
{
	/** Counter to clean the RAM*/
	uint8_t counter;

	/** For CAN0*/
	if(CAN0 == base)
	{
		/** Enables the peripheral clock*/
		PCC->PCCn[PCC_FlexCAN0_INDEX] |= PCC_PCCn_CGC_MASK;
	}

	/** For CAN1*/
	else if(CAN1 == base)
	{
		/** Enables the peripheral clock*/
		PCC->PCCn[PCC_FlexCAN1_INDEX] |= PCC_PCCn_CGC_MASK;
	}

	/** For CAN2*/
	else if(CAN2 == base)
	{
		PCC->PCCn[PCC_FlexCAN2_INDEX] |= PCC_PCCn_CGC_MASK;
	}

	/** Disables the module*/
	base->MCR |= CAN_MCR_MDIS_MASK;
	/** Sets the clock source to the oscillator clock*/
	base->CTRL1 &= (~CAN_CTRL1_CLKSRC_MASK);
	/** Enables the module*/
	base->MCR &= (~CAN_MCR_MDIS_MASK);

	/** Waits for the module to enter freeze mode, to manage the CTRL and other registers*/
	while(!((base->MCR & CAN_MCR_FRZACK_MASK) >> CAN_MCR_FRZACK_SHIFT));

	/** Configures the speed, and other parameters*/
	base->CTRL1 = speed;

	/** Initializes the MB RAM in 0*/
	for(counter = INIT_VAL ; MAX_MSG_BUFFERS > counter ; counter ++)
	{
		base->RAMn[counter] = INIT_VAL;

		/** Sets the ID masks to not check the ID*/
		if(MAX_FILTER_BUFFERS > counter)
		{
			base->RXIMR[counter] = NOT_CHECK_ANY_ID;
		}
	}

	/** Sets the global ID mask to not check any ID*/
	CAN0->RXMGMASK = NOT_CHECK_ANY_ID;

	/** Enables the MB 4 for reception*/
	base->RAMn[(RX_BUFF_OFFSET * MSG_BUF_SIZE) + CODE_AND_DLC_POS] = ENABLE_RX_BUFF;

	/** CAN FD not used*/
	base->MCR = CAN_FD_DISABLE;

	/** Waits for the module to exit freeze mode*/
	while ((base->MCR && CAN_MCR_FRZACK_MASK) >> CAN_MCR_FRZACK_SHIFT);
	/** Waits for the module to be ready*/
	while ((base->MCR && CAN_MCR_NOTRDY_MASK) >> CAN_MCR_NOTRDY_SHIFT);
}

/**************************************************************************************************/

void rtos_start( void )
{
	/* Configure the NVIC, LED outputs and button inputs. */
	prvSetupHardware();

	/* Create the queue. */
	xQueue = xQueueCreate( mainQUEUE_LENGTH, sizeof( unsigned long ) );

	if( xQueue != NULL )
	{
		/* Start the two tasks as described in the comments at the top of this
		file. */

		xTaskCreate( prvQueueReceiveTask, "RX", configMINIMAL_STACK_SIZE, NULL, mainQUEUE_RECEIVE_TASK_PRIORITY, NULL );
		xTaskCreate( prvQueueSendTask, "TX", configMINIMAL_STACK_SIZE, NULL, mainQUEUE_SEND_TASK_PRIORITY, NULL );


		/* Create the software timer that is responsible for turning off the LED
		if the button is not pushed within 5000ms, as described at the top of
		this file. */
		xButtonLEDTimer = xTimerCreate( "ButtonLEDTimer", 			/* A text name, purely to help debugging. */
									mainBUTTON_LED_TIMER_PERIOD_MS,	/* The timer period, in this case 5000ms (5s). */
									pdFALSE,						/* This is a one shot timer, so xAutoReload is set to pdFALSE. */
									( void * ) 0,					/* The ID is not used, so can be set to anything. */
									prvButtonLEDTimerCallback		/* The callback function that switches the LED off. */
								);

		/* Start the tasks and timer running. */
		vTaskStartScheduler();
	}

	/* If all is well, the scheduler will now be running, and the following line
	will never be reached.  If the following line does execute, then there was
	insufficient FreeRTOS heap memory available for the idle and/or timer tasks
	to be created.  See the memory management section on the FreeRTOS web site
	for more details. */
	for( ;; );
}
/*-----------------------------------------------------------*/

static void prvButtonLEDTimerCallback( TimerHandle_t xTimer )
{
	/* Casting xTimer to void because it is unused */
	(void)xTimer;

	/* The timer has expired - so no button pushes have occurred in the last
	five seconds - turn the LED off. */
	GPIO_HAL_SetPins(LED_GPIO, (1 << LED1));
}
/*-----------------------------------------------------------*/

/* The ISR executed when the user button is pushed. */
void vPort_C_ISRHandler( void )
{
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	/* The button was pushed, so ensure the LED is on before resetting the
	LED timer.  The LED timer will turn the LED off if the button is not
	pushed within 5000ms. */
    GPIO_HAL_ClearPins(LED_GPIO, (1 << LED1));
	/* This interrupt safe FreeRTOS function can be called from this interrupt
	because the interrupt priority is below the
	configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY setting in FreeRTOSConfig.h. */
	xTimerResetFromISR( xButtonLEDTimer, &xHigherPriorityTaskWoken );

	/* Clear the interrupt before leaving. */
	PORT_HAL_ClearPortIntFlagCmd(BTN_PORT);

	/* If calling xTimerResetFromISR() caused a task (in this case the timer
	service/daemon task) to unblock, and the unblocked task has a priority
	higher than or equal to the task that was interrupted, then
	xHigherPriorityTaskWoken will now be set to pdTRUE, and calling
	portEND_SWITCHING_ISR() will ensure the unblocked task runs next. */
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}
/*-----------------------------------------------------------*/

static void prvQueueSendTask( void *pvParameters )
{
TickType_t xNextWakeTime;
const unsigned long ulValueToSend = 100UL;

	/* Casting pvParameters to void because it is unused */
	(void)pvParameters;

	/* Initialise xNextWakeTime - this only needs to be done once. */
	xNextWakeTime = xTaskGetTickCount();

	for( ;; )
	{
		/* Place this task in the blocked state until it is time to run again.
		The block time is specified in ticks, the constant used converts ticks
		to ms.  While in the Blocked state this task will not consume any CPU
		time. */
		vTaskDelayUntil( &xNextWakeTime, mainQUEUE_SEND_FREQUENCY_MS );

		/* Send to the queue - causing the queue receive task to unblock and
		toggle an LED.  0 is used as the block time so the sending operation
		will not block - it shouldn't need to block as the queue should always
		be empty at this point in the code. */
		xQueueSend( xQueue, &ulValueToSend, mainDONT_BLOCK );
	}
}
/*-----------------------------------------------------------*/

static void prvQueueReceiveTask( void *pvParameters )
{
unsigned long ulReceivedValue;

	/* Casting pvParameters to void because it is unused */
	(void)pvParameters;

	for( ;; )
	{
		/* Wait until something arrives in the queue - this task will block
		indefinitely provided INCLUDE_vTaskSuspend is set to 1 in
		FreeRTOSConfig.h. */
		xQueueReceive( xQueue, &ulReceivedValue, portMAX_DELAY );

		/*  To get here something must have been received from the queue, but
		is it the expected value?  If it is, toggle the LED. */
		if( ulReceivedValue == 100UL )
		{
		    GPIO_HAL_TogglePins(LED_GPIO, (1 << LED2));
		}
	}
}
/*-----------------------------------------------------------*/

static void prvSetupHardware( void )
{

    boardSetup();

	/* Change LED1, LED2 to outputs. */
	GPIO_HAL_SetPinsDirection(LED_GPIO,  (1 << LED1) | (1 << LED2));

	/* Change BTN1 to input */
	GPIO_HAL_SetPinsDirection(BTN_GPIO, ~(1 << BTN_PIN));

	/* Start with LEDs off. */
	GPIO_HAL_SetPins(LED_GPIO, (1 << LED1) | (1 << LED2));

	/* Install Button interrupt handler */
    INT_SYS_InstallHandler(BTN_PORT_IRQn, vPort_C_ISRHandler, (isr_t *)NULL);
    /* Enable Button interrupt handler */
    INT_SYS_EnableIRQ(BTN_PORT_IRQn);

    /* The interrupt calls an interrupt safe API function - so its priority must
    be equal to or lower than configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY. */
    INT_SYS_SetPriority( BTN_PORT_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY );

}
/*-----------------------------------------------------------*/

void vApplicationMallocFailedHook( void )
{
	/* Called if a call to pvPortMalloc() fails because there is insufficient
	free memory available in the FreeRTOS heap.  pvPortMalloc() is called
	internally by FreeRTOS API functions that create tasks, queues, software
	timers, and semaphores.  The size of the FreeRTOS heap is set by the
	configTOTAL_HEAP_SIZE configuration constant in FreeRTOSConfig.h. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}
/*-----------------------------------------------------------*/

void vApplicationIdleHook( void )
{
    volatile size_t xFreeHeapSpace;

	/* This function is called on each cycle of the idle task.  In this case it
	does nothing useful, other than report the amount of FreeRTOS heap that
	remains unallocated. */
	xFreeHeapSpace = xPortGetFreeHeapSize();

	if( xFreeHeapSpace > 100 )
	{
		/* By now, the kernel has allocated everything it is going to, so
		if there is a lot of heap remaining unallocated then
		the value of configTOTAL_HEAP_SIZE in FreeRTOSConfig.h can be
		reduced accordingly. */
	}

}
/*-----------------------------------------------------------*/

/* The Blinky build configuration does not include run time stats gathering,
however, the Full and Blinky build configurations share a FreeRTOSConfig.h
file.  Therefore, dummy run time stats functions need to be defined to keep the
linker happy. */
void vMainConfigureTimerForRunTimeStats( void ) {}
unsigned long ulMainGetRunTimeCounterValue( void ) { return 0UL; }

/* A tick hook is used by the "Full" build configuration.  The Full and blinky
build configurations share a FreeRTOSConfig.h header file, so this simple build
configuration also has to define a tick hook - even though it does not actually
use it for anything. */
void vApplicationTickHook( void ) {}

/*-----------------------------------------------------------*/
