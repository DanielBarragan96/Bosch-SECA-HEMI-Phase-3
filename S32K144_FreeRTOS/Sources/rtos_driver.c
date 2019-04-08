/*
 * rtos_driver.c
 *
 *  Created on: 03/04/2019
 *      Author: HEMI
 */

#include "rtos_driver.h"

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

#define LED1            15U
#define LED2            16U
#define LED_GPIO        PTD
#define LED_PORT        PORTD
#define LED_PORT_PCC    PCC_PORTD_CLOCK
#define BTN_GPIO        PTC
#define BTN_PIN         13U
#define BTN_PORT        PORTC
#define BTN_PORT_PCC    PCC_PORTC_CLOCK
#define BTN_PORT_IRQn   PORTC_IRQn

#define MB_4_INTERRUPT				(0x10)
#define CLEAR_ALL_FLAGS				(0xFFFFFFFE)


/*********************************************************************************************/

typedef struct {
	uint8_t init_val;
	SemaphoreHandle_t sem_rx_binary;
	SemaphoreHandle_t mutex;
	EventGroupHandle_t event_group;
	QueueHandle_t queue;
}RTOS_CAN_Handler_t;

/*********************************************************************************************/

/*********************************************************************************************/

static RTOS_CAN_Handler_t can_handler = { 0 };
static uint32_t task_period = 100U;
static uint32_t tx_task_period = 1000U;
static uint16_t ADC_value_received = 0;
static uint16_t ADC_threshold_LED = 1500;

static uint8_t ID_SW = 0;
static uint8_t msg_SW[8] = {0};
static uint8_t DLC_SW = 0;


/*********************************************************************************************/

void CAN_RX_Interrupt(void)
{
	if(CAN0->IFLAG1 & MB_4_INTERRUPT)
	{
		xSemaphoreGiveFromISR(can_handler.sem_rx_binary, pdFALSE);
	}

	CAN0->IFLAG1 = CLEAR_ALL_FLAGS;
}

void SW3_ISR(void)
{
	xEventGroupSetBitsFromISR(can_handler.event_group, EVENT_GROUP_SW, pdFALSE);

	/* Clear the interrupt before leaving. */
	PORT_HAL_ClearPortIntFlagCmd(BTN_PORT);
}

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
	/** From here *****************************************************************************/
	SOSC_init_8MHz();       /* Initialize system oscillator for 8 MHz xtal */
	SPLL_init_160MHz();     /* Initialize SPLL to 160 MHz with 8 MHz SOSC */
	NormalRUNmode_80MHz();  /* Init clocks: 80 MHz sysclk & core, 40 MHz bus, 20 MHz flash */
	/** To here *******************************************************************************/

	CAN_Init(base, speed);

#if(!RX_MODE)
	CAN_enable_rx_interruption(base);

	INT_SYS_InstallHandler(CAN0_ORed_0_15_MB_IRQn, CAN_RX_Interrupt, (isr_t *)NULL);
	INT_SYS_EnableIRQ(CAN0_ORed_0_15_MB_IRQn);
	INT_SYS_SetPriority(CAN0_ORed_0_15_MB_IRQn, 3);
#endif

	/*********************** NOTE ***************************/
	/** This module is taken from the driver example FlexCAN*/
	/********************************************************/
	/** From here *****************************************************************************/
	PORT_init();             /* Configure ports */
	LPSPI1_init_master();    /* Initialize LPSPI1 for communication with MC33903 */
	LPSPI1_init_MC33903();   /* Configure SBC via SPI for CAN transceiver operation */
	/** To here *******************************************************************************/

	/**************** LED CONFIGURATION ********************/
	 /* Configure clock source */
	PCC_HAL_SetClockMode(PCC, LED_PORT_PCC, false);
	PCC_HAL_SetClockSourceSel(PCC, LED_PORT_PCC, CLK_SRC_FIRC);
	PCC_HAL_SetClockMode(PCC, LED_PORT_PCC, true);

	PCC_HAL_SetClockMode(PCC, BTN_PORT_PCC, false);
	PCC_HAL_SetClockSourceSel(PCC, BTN_PORT_PCC, CLK_SRC_FIRC);
	PCC_HAL_SetClockMode(PCC, BTN_PORT_PCC, true);

	/* Configure ports */
	PORT_HAL_SetMuxModeSel(LED_PORT, LED1,      PORT_MUX_AS_GPIO);
	PORT_HAL_SetMuxModeSel(LED_PORT, LED2,      PORT_MUX_AS_GPIO);
	PORT_HAL_SetMuxModeSel(BTN_PORT, BTN_PIN,   PORT_MUX_AS_GPIO);
	PORT_HAL_SetPinIntSel(BTN_PORT, BTN_PIN, PORT_INT_RISING_EDGE);

	/* Change LED1, LED2 to outputs. */
	GPIO_HAL_SetPinsDirection(LED_GPIO,  (1 << LED1) | (1 << LED2));

	/* Change BTN1 to input */
	GPIO_HAL_SetPinsDirection(BTN_GPIO, ~(1 << BTN_PIN));

	/* Start with LEDs off. */
	GPIO_HAL_SetPins(LED_GPIO, (1 << LED1) | (1 << LED2));

	/* Install Button interrupt handler */
    INT_SYS_InstallHandler(BTN_PORT_IRQn, SW3_ISR, (isr_t *)NULL);
    /* Enable Button interrupt handler */
    INT_SYS_EnableIRQ(BTN_PORT_IRQn);

    /* The interrupt calls an interrupt safe API function - so its priority must
    be equal to or lower than configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY. */
    INT_SYS_SetPriority( BTN_PORT_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY );
}

void rtos_can_tx_thread_EG(void* args)
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
				CAN_send_message(CAN0, ID_SW, msg_SW, DLC_SW);
			}
		}
	}
}

void rtos_can_tx_thread_periodic(void *args)
{
	uint8_t message_to_send[8] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xFF};
	if(IS_INIT == can_handler.init_val)
	{
		for(;;)
		{
			CAN_send_message(CAN0, 0x123, message_to_send, 8);

			vTaskDelay(pdMS_TO_TICKS(tx_task_period));
		}
	}
}

#if(!RX_MODE)
void rtos_can_rx_thread_interruption(void *args)
{
	uint16_t ID;
	uint8_t msg[8] = { 0 };
	uint8_t DLC;

	if(IS_INIT == can_handler.init_val)
	{
		for(;;)
		{
			xSemaphoreTake(can_handler.sem_rx_binary, portMAX_DELAY);

			CAN_receive_message(CAN0, &ID, msg, &DLC);
		}
	}
}
#endif

#if RX_MODE
void rtos_can_rx_thread_periodic(void *args)
{
	uint16_t ID;
	uint8_t msg[8] = { 0 };
	uint8_t DLC;

	if (IS_INIT == can_handler.init_val)
	{
		for(;;)
		{
			//TODO: Set Rx periodic action
			if(CAN_get_rx_status(CAN0))
			{
				CAN_receive_message(CAN0, &ID, msg, &DLC);
				CAN_clear_tx_and_rx_flags(CAN0);
			}

			vTaskDelay(pdMS_TO_TICKS(task_period));
		}
	}
}
#endif

void rtos_can_LED_control_thread(void *args)
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

void rtos_can_set_sw_msg(uint8_t ID, uint8_t *msg, uint8_t DLC)
{
	uint8_t counter = 0;

	for(counter = 0 ; counter < DLC ; counter ++)
	{
		msg_SW[counter] = (*msg);
		msg ++;
	}

	ID_SW = ID;
	DLC_SW = DLC;
}


