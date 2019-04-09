/*!
 	 \file rtos_driver.hc

 	 \brief This is the source file of the CAN device driver using
 	 	 	 FreeRTOS. All the initialization
 	 	 	 and control functions are found in this source file.

 	 \author HEMI team
 	 	 	 Arpio Fernandez, Leon 				ie702086@iteso.mx
 	 	 	 Barragan Alvarez, Daniel 			ie702554@iteso.mx
 	 	 	 Delsordo Bustillo, Jose Ricardo	ie702570@iteso.mx

 	 \date 	09/04/2019
 */


#include "rtos_driver.h"
#include "ADC.h"

/** Defines the CAN hanlder as initialized*/
#define IS_INIT								(1)
/** Defines the CAN handler as not initialzied*/
#define NOT_INIT							(0)
/** Defines the bits for the ADC event group*/
#define EVENT_GROUP_ADC						(0x01)
/** Defines the bits for the SW3 event group*/
#define EVENT_GROUP_SW						(0x02)

/** Defines the pin for the red LED*/
#define RED_LED_PIN            				(15U)
/** Defines the pin for the green LED*/
#define GREEN_LED_PIN            			(16U)
/** Defines the pin for the blue LED*/
#define BLUE_LED_PIN						(0U)


/** Defines the GPIO for the LEDs*/
#define LED_GPIO        					PTD
/** Defines the Port for the LEDs*/
#define LED_PORT       						PORTD
/** Defines the clock gating for the PORT D*/
#define LED_PORT_PCC    					PCC_PORTD_CLOCK
/** Defines the GPIO for the SW3*/
#define BTN_GPIO        					PTC
/** Defines the pin for the SW3*/
#define BTN_PIN         					13U
/** Defines the port for the SW3*/
#define BTN_PORT        					PORTC
/** Defines the clock gating for the PORT C*/
#define BTN_PORT_PCC    					PCC_PORTC_CLOCK
/** Defines the IRQn for the SW2*/
#define BTN_PORT_IRQn   					PORTC_IRQn

/** Defines the initial value for the variables*/
#define INIT_VAL							(0)

/** Defines the priority for the rx MB interruption*/
#define CAN_RX_INTERRUPT_PRIO				(0x03)
/** Defines a bit to be shifted in masks*/
#define BIT_TO_SHIFT						(1)

/** Defines the interrupt bits of MB4 in IFLAG1*/
#define MB_4_INTERRUPT						(0x10)
/** Defines the bits to clear all IFLAG1 bits*/
#define CLEAR_ALL_FLAGS						(0xFFFFFFFE)

/** Defines the ID of the ADC message*/
#define ADC_TX_ID							(0x10)

/** Defines a mask to get a low byte*/
#define LOW_BYTE_MASK						(0x00FF)
/** Defines a mask to get a high byte*/
#define HIGH_BYTE_MASK						(0xFF00)

/** Defines the ID of the ADC message*/
#define ADC_RX_ID							(0x10)
/** Defines the maximum possible ID*/
#define MAX_ID								(0x7FF)

/** Defines the bit shifts for a byte*/
#define BYTE_SHIFT							(8)
/** Defines the position of the ADC low byte in the ADC vector*/
#define ADC_LOW_BYTE_POS					(0)
/** Defines the position of the ADC high byte in the ADC vector*/
#define ADC_HIGH_BYTE_POS					(1)

/** Defines the ID as not repeated in the ID function vector*/
#define ID_NOT_REPEATED						(0)
/** Defines the ID as repeated in the ID function vector*/
#define ID_REPEATED							(1)

/** Defines the ID as found in the ID function vector*/
#define ID_FOUND							(0)
/** Defines the ID as not found in the ID function vector*/
#define ID_NOT_FOUND						(1)

/** Defines the relation to get the ticks for 1 ms*/
#define FIX_PERIOD							((10.0025F) / (6.0F))

/** Defines the initial period of the Rx task*/
#define RX_TASK_INIT_PERIOD					(100U)
/** Defines the initial period of the Tx task*/
#define TX_TASK_INIT_PERIOD					(1000U)
/** Defines the initial period of the ADC task*/
#define ADC_TX_TASK_INIT_PERIOD				(1000U)

/** Defines the maximum DLC message size*/
#define CAN_MESSAGE_MAX_SIZE				(8)
/** Defines the maximum size of the ID function vector*/
#define ID_VECTOR_MAX_SIZE					(15)

/** Defines the initial threshold of the red LED*/
#define RED_LED_INIT_THRESHOLD				(3750)
/** Defines the initial threshold of the yellow LED*/
#define YELLOW_LED_INIT_THRESHOLD			(2500)
/** Defines the initial threshold of the green LED*/
#define GREEN_LED_INIT_THRESHOLD			(1250)

/** Defines the ADC channel to read the potentiometer*/
#define ADC_POT_CHANNEL						(12)

/** Defines an oversize of the ID function vector*/
#define ID_VECTOR_OVER_SIZE					(200)
/** Defines a position offset of 1 in an array*/
#define ARRAY_POS_OFFSET_1					(1)

/*********************************************************************************************/

/*!
 	 \brief Structure for the RTOS handler.
 */
typedef struct {
	uint8_t init_val;					/*!< Defines whether the handler has been initialized or not*/
	SemaphoreHandle_t sem_rx_binary;	/*!< Binary semaphore for the Rx task*/
	SemaphoreHandle_t mutex;			/*!< Mutex to protect the CAN when sending and receiving*/
	EventGroupHandle_t event_group;		/*!< Event group for the Tx task*/
}RTOS_CAN_Handler_t;

/*********************************************************************************************/

/*********************************************************************************************/

/** RTOS handler for the CAN*/
static RTOS_CAN_Handler_t can_handler = { INIT_VAL };
/** Variable for the rx thread period*/
static uint32_t rx_task_period = RX_TASK_INIT_PERIOD;
/** Variable for the tx thread period*/
static uint32_t tx_task_period = TX_TASK_INIT_PERIOD;
/** Variable for the ADC thread period*/
static uint32_t adc_tx_task_period = ADC_TX_TASK_INIT_PERIOD;

/** ID for the SW3 message*/
static uint8_t ID_SW = INIT_VAL;
/** Message for the SW3*/
static uint8_t msg_SW[CAN_MESSAGE_MAX_SIZE] = {INIT_VAL};
/** DLC of the SW3 message*/
static uint8_t DLC_SW = INIT_VAL;
/** Variable for the value read from the ADC*/
static uint16_t adc_read = INIT_VAL;

/** ID function vector*/
static ID_function_t ID_function[ID_VECTOR_MAX_SIZE] = {{INIT_VAL, NULL}};
/** ID function vector counter*/
static uint8_t ID_func_counter = INIT_VAL;

/** Variable for the received messages*/
static can_message_rx_config_t rx_message;
/** Variable to transmit messages*/
static can_message_tx_config_t tx_message;

/** Variable for the threshold of the red LED*/
static uint16_t red_treshold = RED_LED_INIT_THRESHOLD;
/** Variable for the threshold of the yellow LED*/
static uint16_t yellow_treshold = YELLOW_LED_INIT_THRESHOLD;
/** Variable for the threshold of the green LED*/
static uint16_t green_treshold = GREEN_LED_INIT_THRESHOLD;
/** Variable for the SW3 message*/
static can_message_tx_config_t message_to_send;

/** Variable for the configured CAN base*/
static CAN_Type* can_base;

/*********************************************************************************************/

/** Interruption for the RX message buffer*/
void CAN_RX_Interrupt(void)
{
	/** If the interruption was caused by the MB*/
	if(can_base->IFLAG1 & MB_4_INTERRUPT)
	{
		/** Releases the semaphore to received the data*/
		xSemaphoreGiveFromISR(can_handler.sem_rx_binary, pdFALSE);
	}

	/** Clears the interruption flags*/
	can_base->IFLAG1 = CLEAR_ALL_FLAGS;
}

/** Interruption for the SW3*/
void SW3_ISR(void)
{
	/** Clears the interrupt flags*/
	PORT_HAL_ClearPortIntFlagCmd(BTN_PORT);

	/** Sets the vent group bits*/
	xEventGroupSetBitsFromISR(can_handler.event_group, EVENT_GROUP_SW, pdFALSE);
}

/** This function initializes the RTOS*/
void rtos_can_init(can_init_config_t can_init)
{
	/** Set the handler as initialized*/
	can_handler.init_val = IS_INIT;
	/** Creates the semaphores and the event group*/
	can_handler.sem_rx_binary = xSemaphoreCreateBinary();
	can_handler.mutex = xSemaphoreCreateMutex();
	can_handler.event_group = xEventGroupCreate();

	/*********************** NOTE ***************************/
	/** This module is taken from the driver example FlexCAN*/
	/********************************************************/
	/** From here *****************************************************************************/
	SOSC_init_8MHz();       /* Initialize system oscillator for 8 MHz xtal */
	SPLL_init_160MHz();     /* Initialize SPLL to 160 MHz with 8 MHz SOSC */
	NormalRUNmode_80MHz();  /* Init clocks: 80 MHz sysclk & core, 40 MHz bus, 20 MHz flash */
	/** To here *******************************************************************************/

	/** Sets the configured base*/
	can_base = can_init.base;

	/** Initializes the CAN*/
	CAN_Init(can_init);
	/** Initializes the ADC*/
	ADC_init();

#if(!RX_MODE)
	/** Enables the CAN RX message buffer interruption*/
	CAN_enable_rx_interruption(can_base);

	/** Sets the IRQ hadler, enables it and sets its priority*/
	if(CAN0 == can_base)
	{
		INT_SYS_InstallHandler(CAN0_ORed_0_15_MB_IRQn, CAN_RX_Interrupt, (isr_t *)NULL);
		INT_SYS_EnableIRQ(CAN0_ORed_0_15_MB_IRQn);
		INT_SYS_SetPriority(CAN0_ORed_0_15_MB_IRQn, CAN_RX_INTERRUPT_PRIO);
	}
	else if(CAN1 == can_base)
	{
		INT_SYS_InstallHandler(CAN1_ORed_0_15_MB_IRQn, CAN_RX_Interrupt, (isr_t *)NULL);
		INT_SYS_EnableIRQ(CAN1_ORed_0_15_MB_IRQn);
		INT_SYS_SetPriority(CAN1_ORed_0_15_MB_IRQn, CAN_RX_INTERRUPT_PRIO);
	}
	else if(CAN2 == can_base)
	{
		INT_SYS_InstallHandler(CAN2_ORed_0_15_MB_IRQn, CAN_RX_Interrupt, (isr_t *)NULL);
		INT_SYS_EnableIRQ(CAN2_ORed_0_15_MB_IRQn);
		INT_SYS_SetPriority(CAN2_ORed_0_15_MB_IRQn, CAN_RX_INTERRUPT_PRIO);
	}
#endif

	/*********************** NOTE ***************************/
	/** This module is taken from the driver example FlexCAN,
	 	 and the Blinking_LED example*/
	/********************************************************/
	/** From here *****************************************************************************/
	PORT_init();             /* Configure ports */
	LPSPI1_init_master();    /* Initialize LPSPI1 for communication with MC33903 */
	LPSPI1_init_MC33903();   /* Configure SBC via SPI for CAN transceiver operation */

	/**************** LED CONFIGURATION ********************/
	 /* Configure clock source */
	PCC_HAL_SetClockMode(PCC, LED_PORT_PCC, false);
	PCC_HAL_SetClockSourceSel(PCC, LED_PORT_PCC, CLK_SRC_FIRC);
	PCC_HAL_SetClockMode(PCC, LED_PORT_PCC, true);

	PCC_HAL_SetClockMode(PCC, BTN_PORT_PCC, false);
	PCC_HAL_SetClockSourceSel(PCC, BTN_PORT_PCC, CLK_SRC_FIRC);
	PCC_HAL_SetClockMode(PCC, BTN_PORT_PCC, true);

	/* Configure ports */
	PORT_HAL_SetMuxModeSel(LED_PORT, RED_LED_PIN,      PORT_MUX_AS_GPIO);
	PORT_HAL_SetMuxModeSel(LED_PORT, GREEN_LED_PIN,      PORT_MUX_AS_GPIO);
	PORT_HAL_SetMuxModeSel(BTN_PORT, BTN_PIN,   PORT_MUX_AS_GPIO);
	PORT_HAL_SetPinIntSel(BTN_PORT, BTN_PIN, PORT_INT_RISING_EDGE);

	/* Change RED_LED_PIN, GREEN_LED_PIN to outputs. */
	GPIO_HAL_SetPinsDirection(LED_GPIO,  (BIT_TO_SHIFT << RED_LED_PIN) | (BIT_TO_SHIFT << GREEN_LED_PIN));

	/* Change BTN1 to input */
	GPIO_HAL_SetPinsDirection(BTN_GPIO, ~(BIT_TO_SHIFT << BTN_PIN));

	/* Start with LEDs off. */
	GPIO_HAL_SetPins(LED_GPIO, (BIT_TO_SHIFT << RED_LED_PIN) | (BIT_TO_SHIFT << GREEN_LED_PIN));

	/* Install Button interrupt handler */
    INT_SYS_InstallHandler(BTN_PORT_IRQn, SW3_ISR, (isr_t *)NULL);
    /* Enable Button interrupt handler */
    INT_SYS_EnableIRQ(BTN_PORT_IRQn);

    /* The interrupt calls an interrupt safe API function - so its priority must
    be equal to or lower than configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY. */
    INT_SYS_SetPriority( BTN_PORT_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY );

	/** To here *******************************************************************************/
}

/** CAN tx thread that transmits either the message of the ADC, or the
 	 	 	 message set with rtos_can_set_sw_msg.*/
void rtos_can_tx_thread_EG(void* args)
{
	/** Initializes the ADC message array*/
	uint8_t adc_tx_msg[2] = {INIT_VAL};
	/** Variable to get the event group bits*/
	EventBits_t tx_event;

	/** If the CAN handler has been initialized*/
	if (IS_INIT == can_handler.init_val)
	{
		/** Infinite cycle*/
		for(;;)
		{
			/** Waits for any of the event group bits to be released*/
			xEventGroupWaitBits(can_handler.event_group, EVENT_GROUP_ADC | EVENT_GROUP_SW, pdFALSE, pdFALSE, portMAX_DELAY);
			/** Gets the event group bits*/
			tx_event = xEventGroupGetBits(can_handler.event_group);
			/** Clears the event group bits*/
			xEventGroupClearBits(can_handler.event_group, tx_event);

			/** For the ADC event group*/
			if(EVENT_GROUP_ADC == (tx_event & EVENT_GROUP_ADC))
			{
				/** Sets the ADC read in the message array*/
				adc_tx_msg[ADC_LOW_BYTE_POS] = (uint8_t)(adc_read & LOW_BYTE_MASK);
				adc_tx_msg[ADC_HIGH_BYTE_POS] = (uint8_t)((adc_read & HIGH_BYTE_MASK) >> BYTE_SHIFT);

				/** Sets the values for the tx message*/
				tx_message.base = can_base;
				tx_message.ID = ADC_TX_ID;
				tx_message.msg = adc_tx_msg;
				tx_message.DLC = sizeof(adc_tx_msg);

				/** Sends the message protecting the CAN with a mutex*/
				xSemaphoreTake(can_handler.mutex, portMAX_DELAY);
				CAN_send_message(tx_message);
				xSemaphoreGive(can_handler.mutex);
			}

			/** For the switch event froup*/
			if(EVENT_GROUP_SW == (tx_event & EVENT_GROUP_SW))
			{
				/** Sets the predefined message to the tx message*/
				tx_message.base = can_base;
				tx_message.ID = ID_SW;
				tx_message.msg = msg_SW;
				tx_message.DLC = DLC_SW;

				/** Sends the message protecting the CAN with a mutex*/
				xSemaphoreTake(can_handler.mutex, portMAX_DELAY);
				CAN_send_message(tx_message);
				xSemaphoreGive(can_handler.mutex);
			}
		}
	}
}

/** This thread sends a message periodically.*/
void rtos_can_tx_thread_periodic(void *args)
{
	/** Variable to count the ticks passed since the delay*/
	TickType_t xLastWakeTime;

	/** If the CAN handler has been initialized*/
	if(IS_INIT == can_handler.init_val)
	{
		/** Gets the current ticks*/
		xLastWakeTime = xTaskGetTickCount();

		/** Infinite cycle*/
		for(;;)
		{
			/** Sets the predefined tx message*/
			tx_message.base = message_to_send.base;
			tx_message.ID = message_to_send.ID;
			tx_message.msg = message_to_send.msg;
			tx_message.DLC = message_to_send.DLC;

			/** Sends the message protecting the CAN with a mutex*/
			xSemaphoreTake(can_handler.mutex, portMAX_DELAY);
			CAN_send_message(tx_message);
			xSemaphoreGive(can_handler.mutex);

			/** Delay to make the function periodical*/
			vTaskDelayUntil(&xLastWakeTime, (tx_task_period * FIX_PERIOD));
		}
	}
}

#if(!RX_MODE)
/** This thread receives a message using interruption.*/
void rtos_can_rx_thread_interruption(void *args)
{
	/** Variable for the received ADC value*/
	uint16_t received_ADC_val = INIT_VAL;
	/** Counter for the ID function vector*/
	uint8_t ID_counter = INIT_VAL;

	/** If the CAN handler has been initialized*/
	if(IS_INIT == can_handler.init_val)
	{
		/** Infinite cycle*/
		for(;;)
		{
			/** Takes the interruption semaphore*/
			xSemaphoreTake(can_handler.sem_rx_binary, portMAX_DELAY);

			/** Sets the base*/
			rx_message.base = can_base;

			/** Receives a message protecting CAN*/
			xSemaphoreTake(can_handler.mutex, portMAX_DELAY);
			CAN_receive_message(&rx_message);
			xSemaphoreGive(can_handler.mutex);

			/** Checks the received IDs*/
			switch(rx_message.ID)
			{
				/** Specific case for the ADC ID*/
				case ADC_RX_ID:
					/** Sets the value received into one variable*/
					received_ADC_val = (uint16_t)(rx_message.msg[ADC_LOW_BYTE_POS]);
					received_ADC_val |= (uint16_t)(rx_message.msg[ADC_HIGH_BYTE_POS] << BYTE_SHIFT);

					/** Turns on the LED according to the received ADC value*/
					rtos_turn_on_leds(received_ADC_val);
				break;

				/** For any other ID*/
				default:
					/** Checks the ID function vector (Only the initialized IDs)*/
					for(ID_counter = INIT_VAL ; ID_counter < ID_func_counter ; ID_counter ++)
					{
						/** If the received ID exists in the ID function vector*/
						if(rx_message.ID == ID_function[ID_counter].ID)
						{
							/** Calls the corresponding function*/
							ID_function[ID_counter].ID_func(rx_message);
						}
					}
				break;
			}
		}
	}
}
#endif

#if(RX_MODE)
/** This thread receives a message, by checking the RX flag
 	 periodically (Polling). The default period is 100ms.*/
void rtos_can_rx_thread_periodic(void *args)
{
	/** Counter for the ID vector*/
	uint8_t ID_counter = INIT_VAL;
	/** Variable for the received ADC*/
	uint16_t received_ADC_val = INIT_VAL;
	/** Variable to count the ticks passed since the delay*/
	TickType_t xLastWakeTime;

	/** If the CAN handler has been initialized*/
	if(IS_INIT == can_handler.init_val)
	{
		/** Gets the current tick count*/
		xLastWakeTime = xTaskGetTickCount();

		/** Infinite cycle*/
		for(;;)
		{
			/** Queries the status of the Rx interruption*/
			if(CAN_get_rx_status(can_base))
			{
				/** Gets the configured bas*/
				rx_message.base = can_base;

				/** Receives the message protecting the CAN first*/
				xSemaphoreTake(can_handler.mutex, portMAX_DELAY);
				CAN_receive_message(&rx_message);
				xSemaphoreGive(can_handler.mutex);

				/** Clears the interruption flags*/
				CAN_clear_tx_and_rx_flags(rx_message.base);

				/** Checks the received ID*/
				switch(rx_message.ID)
				{
					/** For the ADC ID*/
					case ADC_RX_ID:
						/** Concatenates the ADC received into a variable*/
						received_ADC_val = (uint16_t)(rx_message.msg[ADC_LOW_BYTE_POS]);
						received_ADC_val |= (uint16_t)(rx_message.msg[ADC_HIGH_BYTE_POS] << BYTE_SHIFT);

						/** Turns the LED on according to the ADC received*/
						rtos_turn_on_leds(received_ADC_val);
					break;

					/** For any other ID*/
					default:
						/** Checks the ID function vector*/
						for(ID_counter = INIT_VAL ; ID_counter < ID_func_counter ; ID_counter ++)
						{
							/** If the ID received exists in the function vector*/
							if(rx_message.ID == ID_function[ID_counter].ID)
							{
								/** Calls the corresponding function*/
								ID_function[ID_counter].ID_func(rx_message);
							}
						}
					break;
				}
			}

			/** Delay to make the function periodic*/
			vTaskDelayUntil(&xLastWakeTime, (rx_task_period * FIX_PERIOD));
		}
	}
}
#endif

/** This function sets the message to be sent when a SW3 interruption occurrs*/
void rtos_can_set_sw_msg(can_message_tx_config_t can_message_tx)
{
	/** Counter to copy the vector*/
	uint8_t counter = INIT_VAL;

	for(counter = INIT_VAL ; counter < can_message_tx.DLC ; counter ++)
	{
		/** Copies each value of the message to the tx message*/
		msg_SW[counter] = (*can_message_tx.msg);
		can_message_tx.msg ++;
	}

	/** Sets the ID and the DLC to the tx message*/
	ID_SW = can_message_tx.ID;
	DLC_SW = can_message_tx.DLC;
}

/** This function receives from CAN protecting it with mutex*/
void rtos_can_receive(can_message_rx_config_t *can_message_tx)
{
	/** Takes the mutex*/
	xSemaphoreTake(can_handler.mutex, portMAX_DELAY);
	/** Receives the message*/
	CAN_receive_message(can_message_tx);
	/** Releases the mutex*/
	xSemaphoreGive(can_handler.mutex);
}

/** This function transmits from CAN protecting it with mutex*/
void rtos_can_transmit(can_message_tx_config_t can_message_tx)
{
	/** Takes the mutex*/
	xSemaphoreTake(can_handler.mutex, portMAX_DELAY);
	/** Sends the message*/
	CAN_send_message(can_message_tx);
	/** Releases the mutex*/
	xSemaphoreGive(can_handler.mutex);
}

/** This function reads periodically the ADC*/
void rtos_adc_read_thread(void *args)
{
	/** Variable to count the ticks passed since the delay*/
	TickType_t xLastWakeTime;

	/** If the handler has been initialized*/
	if(IS_INIT == can_handler.init_val)
	{
		/** Gets the current ticks count*/
		xLastWakeTime = xTaskGetTickCount();

		/** Infinite cycle*/
		for(;;)
		{
			/** Converts the ADC value from the potentiometer channel*/
			convertAdcChan(ADC_POT_CHANNEL);

			/** Waits for the ADC to finish the conversion*/
			while(0 == adc_complete());
			/** Reads the ADC*/
			adc_read = read_adc_chx();

			/** Releases the event group*/
			xEventGroupSetBits(can_handler.event_group, EVENT_GROUP_ADC);

			/** Delay to make the task periodically*/
			vTaskDelayUntil(&xLastWakeTime, (adc_tx_task_period * FIX_PERIOD));
		}
	}
}

/** This function turns on the LEDs according to the ADC value received from CAN*/
void rtos_turn_on_leds(uint16_t adc_received)
{
	/** For values bigger than the red threshold*/
    if (adc_received > red_treshold)
    {
    	/** Turns on the red LED*/
    	turn_on_red_LED();
    }

	/** For values bigger than the yellow threshold*/
    else if (adc_received > yellow_treshold)
    {
    	/** Turns on the yellow LED*/
    	turn_on_yellow_LED();
    }

	/** For values bigger than the green threshold*/
    else if (adc_received > green_treshold)
    {
    	/** Turns on the green LED*/
    	turn_on_green_LED();
    }

	/** For other value*/
    else
    {
    	/** Turns off the LEDs*/
    	turn_off_LEDS();
    }
}

/** This function sets the period for the RX thread*/
void set_rx_thread_period(uint32_t new_value)
{
	rx_task_period = new_value;
}

/** This function sets the period for the TX thread*/
void set_tx_thread_period(uint32_t new_value)
{
	tx_task_period = new_value;
}

/** This function sets the period for the ADC thread*/
void set_adc_tx_thread_period(uint32_t new_value)
{
	adc_tx_task_period = new_value;
}

/** This function turns on the red LED, turning off other LEDs*/
void turn_on_red_LED()
{
	/*********************** NOTE ***************************/
	/** This function is taken from the example Blinking_LED*/
	/********************************************************/
	PTD->PSOR |= BIT_TO_SHIFT << BLUE_LED_PIN | BIT_TO_SHIFT << GREEN_LED_PIN;    /* turn off blue, green LEDs */
	PTD->PCOR |= BIT_TO_SHIFT << RED_LED_PIN;              /* turn on red LED */
}

/** This function turns on the green LED turning off other LEDs*/
void turn_on_green_LED()
{
	/*********************** NOTE ***************************/
	/** This function is taken from the example Blinking_LED*/
	/********************************************************/
	PTD->PSOR |= BIT_TO_SHIFT << BLUE_LED_PIN | BIT_TO_SHIFT << RED_LED_PIN;    /* turn off blue, red LEDs */
	PTD->PCOR |= BIT_TO_SHIFT << GREEN_LED_PIN;     	      /* turn on green LED */
}

/** This function turns on the green and red LED, turning off the blue LED*/
void turn_on_yellow_LED()
{
	/*********************** NOTE ***************************/
	/** This function is taken from the example Blinking_LED*/
	/********************************************************/
	turn_off_LEDS();
	PTD->PCOR |= BIT_TO_SHIFT << RED_LED_PIN;             /* turn on red LED */
	PTD->PCOR |= BIT_TO_SHIFT << GREEN_LED_PIN;    	      /* turn on green LED */
}

/** This function turns off all the LEDs*/
void turn_off_LEDS()
{
	/*********************** NOTE ***************************/
	/** This function is taken from the example Blinking_LED*/
	/********************************************************/
	PTD->PSOR |= BIT_TO_SHIFT << BLUE_LED_PIN | BIT_TO_SHIFT <<  RED_LED_PIN | BIT_TO_SHIFT << GREEN_LED_PIN; /* Turn off all LEDs */
}

/** This function adds an ID and a function to the ID function vector*/
ID_func_vector_state_t rtos_add_ID_function(ID_function_t ID_func)
{
	/** Sets the return value as successful*/
	ID_func_vector_state_t retval = ID_func_vector_success;
	/** Counter for the ID vector*/
	uint8_t ID_counter = INIT_VAL;
	/** Variable to check whether the ID already exists in the vector or not*/
	uint8_t ID_repeated = ID_NOT_REPEATED;

	/** If the ID function vector is full*/
	if(15 <= ID_func_counter)
	{
		/** Sets the return value to full*/
		retval = ID_func_vector_full;
	}

	/** If the ID is outside of the limits
	 	 The limits used were the following
	 	 	 ADC_RX_ID as the highest priority, for the lower limit
	 	 	 11-bit value for the upper limit*/
	else if(ADC_RX_ID >= ID_func.ID || MAX_ID < ID_func.ID)
	{
		/** Sets the ID as not allowed*/
		retval = ID_not_allowed;
	}

	/** The vector can receive the ID*/
	else
	{
		/** Checks the existing IDs*/
		for(ID_counter = INIT_VAL ; ID_counter < ID_func_counter ; ID_counter ++)
		{
			/** If an ID already exists*/
			if(ID_func.ID == ID_function[ID_counter].ID)
			{
				/** Sets the ID as repeated*/
				ID_repeated = ID_REPEATED;
			}
		}

		/** If the ID is not repeated*/
		if(ID_NOT_REPEATED == ID_repeated)
		{
			/** Saves the ID and the function in the vector*/
			ID_function[ID_func_counter].ID = ID_func.ID;
			ID_function[ID_func_counter].ID_func = ID_func.ID_func;

			/** Incremetns the size of the vector*/
			ID_func_counter ++;
		}

		/** If the ID is repeated*/
		else
		{
			/** Sets the return value as existing ID*/
			retval = ID_already_exist;
		}
	}

	return retval;
}

/** This function removes an ID from the ID function vector*/
ID_func_vector_state_t rtos_remove_ID_function(ID_function_t ID_func)
{
	/** Sets the return value as successful*/
	ID_func_vector_state_t retval = ID_func_vector_success;
	/** Counter for the ID vector*/
	uint8_t ID_counter = INIT_VAL;
	/** Variable to set which ID to erase*/
	uint8_t ID_to_erase = ID_VECTOR_OVER_SIZE;

	/** If the vector is empty*/
	if(INIT_VAL >= ID_func_counter)
	{
		/** Sets the return value as empty*/
		retval = ID_func_vector_empty;
	}

	/** If the ID is outside of the limits
	 	 The limits used were the following
	 	 	 ADC_RX_ID as the highest priority, for the lower limit
	 	 	 11-bit value for the upper limit*/
	else if(ADC_RX_ID >= ID_func.ID || MAX_ID < ID_func.ID)
	{
		retval = ID_not_allowed;
	}

	/** If the vector can receive the ID*/
	else
	{
		/** Checks the IDs in the vector*/
		for(ID_counter = INIT_VAL ; ID_counter < ID_func_counter ; ID_counter ++)
		{
			/** IF the ID exists*/
			if(ID_func.ID == ID_function[ID_counter].ID)
			{
				/** Sets ID to erase index*/
				ID_to_erase = ID_counter;
			}
		}

		/** If the ID to erase didn't change*/
		if(ID_VECTOR_OVER_SIZE == ID_to_erase)
		{
			/** Sets the ID as non-existing*/
			retval = ID_does_not_exist;
		}

		/** Otherwise*/
		else
		{
			/** Erases the ID*/
			ID_function[ID_to_erase].ID = INIT_VAL;
			ID_function[ID_to_erase].ID_func = NULL;

			/** Moves all the IDs from the erased one, one position in the vector forward*/
			for(ID_counter = ID_to_erase ; ID_counter < (ID_func_counter - ARRAY_POS_OFFSET_1) ; ID_counter ++)
			{
				ID_function[ID_counter].ID = ID_function[ID_counter + ARRAY_POS_OFFSET_1].ID;
				ID_function[ID_counter].ID_func = ID_function[ID_counter + ARRAY_POS_OFFSET_1].ID_func;
			}

			/** Deletes the repeated ID and function*/
			ID_function[ID_counter].ID = INIT_VAL;
			ID_function[ID_counter].ID_func = NULL;

			/** Decreases the vector size*/
			ID_func_counter --;
		}
	}

	return retval;
}

/** This function changes and ID and a function in the ID vector function*/
ID_func_vector_state_t rtos_change_ID_function(ID_function_t ID_func_old, ID_function_t ID_func_new)
{
	/** Sets the return value as success*/
	ID_func_vector_state_t retval = ID_func_vector_success;
	/** Counter for the ID function vector*/
	uint8_t ID_counter = INIT_VAL;
	/** Variable to set whether the ID was found or not*/
	uint8_t ID_found = ID_NOT_FOUND;

	/** If the new ID is outside of the limits
	 	 The limits used were the following
	 	 	 ADC_RX_ID as the highest priority, for the lower limit
	 	 	 11-bit value for the upper limit*/
	if(ID_func_new.ID || MAX_ID < ID_func_new.ID)
	{
		/** Sets the return value as ID not allowed*/
		retval = ID_not_allowed;
	}

	/** If the vector can receive the ID*/
	else
	{
		/** Checks every ID in the vector*/
		for(ID_counter = INIT_VAL ; ID_counter < ID_func_counter ; ID_counter ++)
		{
			/** If the target ID is found in the vector*/
			if(ID_func_old.ID == ID_function[ID_counter].ID)
			{
				/** Changes the ID and the function*/
				ID_function[ID_counter].ID = ID_func_new.ID;
				ID_function[ID_counter].ID_func = ID_func_new.ID_func;

				/** Sets the ID as found*/
				ID_found = ID_FOUND;
			}
		}

		/** If the ID was not found*/
		if(ID_NOT_FOUND == ID_found)
		{
			/** Sets the return value as non-existing ID*/
			retval = ID_does_not_exist;
		}
	}

	return retval;
}

/** This function returns the ID function vector size*/
uint8_t rtos_get_ID_function_vector_size(void)
{
	return ID_func_counter;
}

/** This function sets the LED thresholds*/
void LED_treshold_values(uint16_t red, uint16_t yellow, uint16_t green)
{
	/** If the new red threshold is not 0*/
	if(INIT_VAL != red)
	{
		/** Sets the threshold*/
		red_treshold = red;
	}

	/** If the new green threshold is not 0*/
	if(INIT_VAL != green)
	{
		/** Sets the threshold*/
		green_treshold = green;
	}

	/** If the new yellow threshold is not 0*/
	if(INIT_VAL != yellow)
	{
		/** Sets the threshold*/
		yellow_treshold = yellow;
	}
}

/** This function sets the periodic message for TX*/
void rtos_define_tx_periodic_msg(can_message_tx_config_t can_message_tx)
{
	/** Copies each value into the tx message structure*/
	message_to_send.base = can_message_tx.base;
	message_to_send.ID = can_message_tx.ID;
	message_to_send.msg = can_message_tx.msg;
	message_to_send.DLC = can_message_tx.DLC;
}
