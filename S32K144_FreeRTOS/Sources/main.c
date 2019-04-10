/*
 * Copyright (c) 2015 - 2016 , Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * THIS SOFTWARE IS PROVIDED BY NXP "AS IS" AND ANY EXPRESSED OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL NXP OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
 * THE POSSIBILITY OF SUCH DAMAGE.
 */

/* ###################################################################
**     Filename    : main.c
**     Project     : freertos
**     Processor   : S32K144_100
**     Version     : Driver 01.00
**     Compiler    : GNU C Compiler
**     Date/Time   : 2015-07-03, 14:05, # CodeGen: 0
**     Abstract    :
**         Main module.
**         This module contains user's application code.
**     Settings    :
**     Contents    :
**         No public methods
**
** ###################################################################*/
/*!
** @file main.c
** @version 01.00
** @brief
**         Main module.
**         This module contains user's application code.
*/
/*!
**  @addtogroup main_module main module documentation
**  @{
*/
/* MODULE main */


/* Including needed modules to compile this module/procedure */
#include "Cpu.h"
#include "clockMan1.h"
#include "pin_mux.h"
#include "FreeRTOS.h"
#if CPU_INIT_CONFIG
  #include "Init_Config.h"
#endif
#include "S32K144.h"

#include "transceiver.h"
#include "clocks_and_modes.h"
#include "rtos_driver.h"
#include "task.h"

volatile int exit_code = 0;
/* User includes (#include below this line is not maintained by Processor Expert) */

#include <stdint.h>
#include <stdbool.h>

/** ID for the SW3 message*/
#define SW3_MSG_ID				(0x30)
/** ID for the periodic tx message*/
#define PERIODIC_MSG_ID			(0x40)
/** ID for the RX callback*/
#define TEST_CALLBACK_ID		(0x123)

/** RX thread priority*/
#define RX_THREAD_PRIO			(3)
/** ADC thread priority*/
#define ADC_THREAD_PRIO			(4)
/** TX thread priority*/
#define TX_THREAD_PRIO			(5)

/** Period for the TX thread*/
#define TX_THREAD_PERIOD		(1000)
/** Period for the ADC thread*/
#define ADC_THREAD_PERIOD		(250)

/** Test callback function*/
void test_function(can_message_rx_config_t can_message_rx)
{
	/** Variable to send a message*/
	can_message_tx_config_t msg_test_function;
	/** Message to be sent*/
	uint8_t msg[4] = {0x01, 0x23, 0x45, 0x67};

	/** Sets the values for the tx message*/
	msg_test_function.base = CAN0;
	msg_test_function.ID = 0x25;
	msg_test_function.msg = msg;
	msg_test_function.DLC = sizeof(msg);

	/** Sends the message protecting it with mutex*/
	rtos_can_transmit(msg_test_function);
}

int main(void)
{
	/** SW3 message*/
	uint8_t msg[7] = {0x08, 0x00, 0x00, 0x08, 0x01, 0x03, 0x05};
	/** Periodic message*/
	static uint8_t per_msg[3] = {0x01, 0x23, 0x45};
	/** Test ID to be set to the ID function vector*/
	ID_function_t test_ID_func;

	/** Variable to initialize the CAN*/
	can_init_config_t can_init;
	/** SW3 message structure*/
	can_message_tx_config_t tx_msg_init;
	/** Periodic message structure*/
	static can_message_tx_config_t periodic_msg;

	/** Sets the base and the speed for CAN*/
	can_init.base = CAN0;
	can_init.speed = CAN_CTRL1_SPEED_500KBPS;

	/** Sets the SW3 message*/
	tx_msg_init.base = CAN0;
	tx_msg_init.ID = SW3_MSG_ID;
	tx_msg_init.msg = msg;
	tx_msg_init.DLC = sizeof(msg);

	/** Sets the periodic message*/
	periodic_msg.base = CAN0;
	periodic_msg.ID = PERIODIC_MSG_ID;
	periodic_msg.msg = per_msg;
	periodic_msg.DLC = sizeof(per_msg);

	/** Sets the ID and the callback function*/
	test_ID_func.ID = TEST_CALLBACK_ID;
	test_ID_func.ID_func = test_function;

	/** Defines the tx messages (Periodic and SW3*/
	rtos_define_tx_periodic_msg(periodic_msg);
	rtos_can_set_sw_msg(tx_msg_init);

	/** Adds the RX ID and function*/
	rtos_add_ID_function(test_ID_func);

	/** Sets the periods for tx and ADC*/
	set_tx_thread_period(TX_THREAD_PERIOD);
	set_adc_tx_thread_period(ADC_THREAD_PERIOD);

	/** Initializes the rtos can*/
	rtos_can_init(can_init);

	/** Creates the TX thread by interrupt*/
	sys_thread_new("TX_interrupt_thread", rtos_can_tx_thread_EG, NULL, configMINIMAL_STACK_SIZE, TX_THREAD_PRIO);
	/** Creates the TX periodic thread*/
	sys_thread_new("TX_periodic_thread", rtos_can_tx_thread_periodic, NULL, configMINIMAL_STACK_SIZE, TX_THREAD_PRIO);

	/*******************************************************************************************************************/
	/** NOTE: To test both the periodic RX and the RX by interrupt, please the value of RX_MODE, found in rtos_driver.h*/
	/*******************************************************************************************************************/
#if(!RX_MODE)
	/** Creates the RX thread by interrupt*/
	sys_thread_new("RX", rtos_can_rx_thread_interruption, NULL, configMINIMAL_STACK_SIZE, RX_THREAD_PRIO);
#endif
#if(RX_MODE)
	/** Creates the RX periodic thread*/
	sys_thread_new("RX", rtos_can_rx_thread_periodic, NULL, configMINIMAL_STACK_SIZE, RX_THREAD_PRIO);
#endif

	/** Creates the ADC thread*/
	sys_thread_new("ADC", rtos_adc_read_thread, NULL, configMINIMAL_STACK_SIZE, ADC_THREAD_PRIO);

	/* Start the tasks and timer running. */
	vTaskStartScheduler();

	for(;;);

	return 0;
}
/* END main */
/*!
** @}
*/
/*
** ###################################################################
**
**     This file was created by Processor Expert 10.1 [05.21]
**     for the Freescale S32K series of microcontrollers.
**
** ###################################################################
*/
