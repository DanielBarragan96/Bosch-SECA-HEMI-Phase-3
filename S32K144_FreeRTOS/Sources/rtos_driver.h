/*
 * rtos_driver.h
 *
 *  Created on: 03/04/2019
 *      Author: lei-n
 */

#ifndef RTOS_DRIVER_H_
#define RTOS_DRIVER_H_

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

/* RTOS includes. */
#include "projdefs.h"
#include "can_driver.h"
#include "semphr.h"
#include "event_groups.h"

/* Drivers include. */
#include "transceiver.h"
#include "clocks_and_modes.h"

#define RX_INTERRUPT						(0)
#define RX_PERIODIC							(1)

#define RX_MODE								RX_INTERRUPT

#define mainQUEUE_RECEIVE_TASK_PRIORITY		( tskIDLE_PRIORITY + 2 )
#define	mainQUEUE_SEND_TASK_PRIORITY		( tskIDLE_PRIORITY + 1 )

void rtos_start(CAN_Type* base, uint32_t speed);

void rtos_can_tx_thread_EG(void* args);

void rtos_can_tx_thread_periodic(void *args);

void rtos_can_rx_thread_periodic(void *args);

#if(!RX_MODE)
void rtos_can_rx_thread_interruption(void *args);
#endif

#if RX_MODE
void rtos_can_rx_thread_periodic(void *args);
#endif
void rtos_can_LED_control_thread(void *args);

void rtos_can_set_sw_msg(uint8_t ID, uint8_t *msg, uint8_t DLC);

#endif /* RTOS_DRIVER_H_ */
