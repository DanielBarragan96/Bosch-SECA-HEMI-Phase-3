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

#include "BoardDefines.h"
#include "projdefs.h"
#include "can_driver.h"
#include "semphr.h"

#include "transceiver.h"
#include "clocks_and_modes.h"


void rtos_start(CAN_Type* base, uint32_t speed);

void rtos_can_tx_thread(void* args);

#endif /* RTOS_DRIVER_H_ */
