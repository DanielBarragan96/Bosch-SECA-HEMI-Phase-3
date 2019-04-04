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
#include "can_driver.h"

#define SEND_ID				(0x511)

volatile int exit_code = 0;
/* User includes (#include below this line is not maintained by Processor Expert) */

#include <stdint.h>
#include <stdbool.h>

extern void rtos_start(void);
#define PEX_RTOS_START rtos_start

/*!
  \brief The main function for the project.
  \details The startup initialization sequence is the following:
 * - __start (startup asm routine)
 * - __init_hardware()
 * - main()
 *   - PE_low_level_init()
 *     - Common_Init()
 *     - Peripherals_Init()
*/
int main(void)
{
	/**Disables Watchdog*/
	WDOG_disable();

	/*********************** NOTE ***************************/
	/** This module is taken from the driver example FlexCAN*/
	/********************************************************/
	/** From here **********************************************************************************************/
	SOSC_init_8MHz();       /* Initialize system oscillator for 8
	   MHz xtal */
	SPLL_init_160MHz();     /* Initialize SPLL to 160 MHz with 8 MHz SOSC */
	NormalRUNmode_80MHz();  /* Init clocks: 80 MHz sysclk & core, 40 MHz bus, 20 MHz flash */
	/** To here **********************************************************************************************/

	CAN_Init(CAN0, CAN_CTRL1_SPEED_500KBPS);

	/*********************** NOTE ***************************/
	/** This module is taken from the driver example FlexCAN*/
	/********************************************************/
	/** From here **********************************************************************************************/
	PORT_init();             /* Configure ports */
	LPSPI1_init_master();    /* Initialize LPSPI1 for communication with MC33903 */
	LPSPI1_init_MC33903();   /* Configure SBC via SPI for CAN transceiver operation */
	/** To here **********************************************************************************************/

  /*** Processor Expert internal initialization. DON'T REMOVE THIS CODE!!! ***/
  #ifdef PEX_RTOS_INIT
    PEX_RTOS_INIT();                 /* Initialization of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of Processor Expert internal initialization.                    ***/

  /* All of the code is in rtos.c file */

  /*** Don't write any code pass this line, or it will be deleted during code generation. ***/
  /*** RTOS startup code. Macro PEX_RTOS_START is defined by the RTOS component. DON'T MODIFY THIS CODE!!! ***/
  #ifdef PEX_RTOS_START
    PEX_RTOS_START();                  /* Startup of the selected RTOS. Macro is defined by the RTOS component. */
  #endif
  /*** End of RTOS startup code.  ***/

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
