/*!
 	 \file rtos_driver.h

 	 \brief This is the header file of the CAN device driver using
 	 	 	 FreeRTOS. All the initialization
 	 	 	 and control functions are found in this header.

 	 \author HEMI team
 	 	 	 Arpio Fernandez, Leon 				ie702086@iteso.mx
 	 	 	 Barragan Alvarez, Daniel 			ie702554@iteso.mx
 	 	 	 Delsordo Bustillo, Jose Ricardo	ie702570@iteso.mx

 	 \date 	09/04/2019
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

/** Defines the RX thread to work by semaphores (Aperiodically)*/
#define RX_INTERRUPT						(0)
/** Defines the RX thread to work periodically*/
#define RX_PERIODIC							(1)

/** Sets the mode of the RX thread*/
#define RX_MODE								RX_INTERRUPT

/*!
 	 \brief Enumerator to define the states of the ID function vector.
 */
typedef enum
{
	ID_func_vector_success,	/*!< ID vector configuration successful*/
	ID_func_vector_full,	/*!< ID vector is full*/
	ID_func_vector_empty,	/*!< ID vector is empty*/
	ID_not_allowed,			/*!< ID not allowed to be set*/
	ID_does_not_exist,		/*!< ID does not exists in the ID vector*/
	ID_already_exist		/*!< ID already exists in the ID vector*/
}ID_func_vector_state_t;

/*!
 	 \brief Structure to define the ID vector.
 */
typedef struct
{
	uint16_t ID;												/*!< ID to be stored*/
	void (*ID_func)(can_message_rx_config_t can_message_rx);	/*!< Pointer to the function to be executed*/
}ID_function_t;

/*!
 	 \brief This function initializes the handlers and drivers necessary to run
 	 	 	 CAN in FreeRTOS.

 	 \param[in] can_init Configuration for the CAN driver.

 	 \return void.
 */
void rtos_can_init(can_init_config_t can_init);

/*!
 	 \brief CAN tx thread that transmits either the message of the ADC, or the
 	 	 	 message set with rtos_can_set_sw_msg.

 	 \note The ADC message is sent periodically, and the SW message is sent
 	 	 	 when SW3 is pressed.

 	 \note The ADC message period can be set with set_adc_tx_thread_period.

 	 \param[in] args Thread arguments. Set to NULL.

 	 \return void.
 */
void rtos_can_tx_thread_EG(void* args);

/*!
 	 \brief This thread sends a message periodically.

 	 \note The message sent by this thread is set with rtos_define_tx_periodic_msg.
 	 	 	 Please set it before using this function. The default period is 1s.

 	 \param[in] args Thread arguments. Set to NULL.

 	 \return void.
 */
void rtos_can_tx_thread_periodic(void *args);

/*!
 	 \brief This function sets the message to be sent periodically.

 	 \param[in] can_message_tx Structure of the message to be sent.

 	 \return void.
 */
void rtos_define_tx_periodic_msg(can_message_tx_config_t can_message_tx);

/*!
 	 \brief This function sets the period of the Tx thread.

 	 \param[in] new_value New period, in milliseconds, of the Tx thread.

 	 \return void.
 */
void set_tx_thread_period(uint32_t new_value);

#if(!RX_MODE)
/*!
 	 \brief This thread receives a message using interruption.

 	 \note Use rtos_add_ID_function or rtos_change_ID_function to set a callback
 	 	 	 for when a certain ID is received.

 	 \param[in] args Thread arguments. Set to NULL.

 	 \return void.
 */
void rtos_can_rx_thread_interruption(void *args);
#endif

#if RX_MODE
/*!
 	 \brief This thread receives a message, by checking the RX flag
 	 	 	 periodically (Polling). The default period is 100ms.

 	 \param[in] args Thread arguments. Set to NULL.

 	 \return void.
 */
void rtos_can_rx_thread_periodic(void *args);

/*!
 	 \brief This function sets the period of the RX thread.

 	 \param[in] new_value New period, in milliseconds, of the RX thread.

 	 \return void.
 */
void set_rx_thread_period(uint32_t new_value);
#endif

/*!
 	 \brief This thread reads the ADC periodically, and sets the EG for the value
 	 	 	 to be sent.

 	 \param[in] args Thread arguments. Set to NULL.

 	 \return void.
 */
void rtos_adc_read_thread(void *args);

/*!
 	 \brief This function sets the period of the ADC thread.

 	 \param[in] new_value New period, in milliseconds, of the ADC thread.
 */
void set_adc_tx_thread_period(uint32_t new_value);


/*!
 	 \brief This function sets the message to be sent when the SW3 is pressed.

 	 \param[in] can_message_tx Structure of the message to be sent.

 	 \return void.
 */
void rtos_can_set_sw_msg(can_message_tx_config_t can_message_tx);

/*!
 	 \brief This function receives a message protecting the CAN with a mutex.

 	 \param[out] can_message_rx Message structure with the data received.

 	 \note can_message_tx.base is actually param[in], so it must be set before calling the function.

 	 \return void.
 */
void rtos_can_receive(can_message_rx_config_t *can_message_tx);

/*!
 	 \brief This function transmits a message protecting the CAN wit a mutex.

 	 \param[in] can_message_tx Message structure with the data to be transmitted.

 	 \return void
 */
void rtos_can_transmit(can_message_tx_config_t can_message_tx);

/*!
 	 \brief This function turns on the LEDs according to the thresholds set for
 	 	 	 each color LED.

 	 \param[in] adc_received Value to decide which LED will be turned on.

 	 \return void.
 */
void rtos_turn_on_leds(uint16_t adc_received);

/*!
 	 \brief This function adds an ID and a callback to be executed when the ID set
 	 	 	 received.

 	 \note The maximum IDs that can be stored are 15.

 	 \param[in] ID_func ID and callback function to be stored.

 	 \return This function indicates if the task was successful, or if an error occurred.
 */
ID_func_vector_state_t rtos_add_ID_function(ID_function_t ID_func);

/*!
 	 \brief THis function removes and ID and its respective callback from the ID vector.

 	 \param[in] ID_func ID and callback to be removed.

 	 \warning This function only checks for IDs, if an ID is found in the vector, it will
 	 	 	 	 remove the ID and the callback, regardless of whether the function is
 	 	 	 	 "correct" or not.

 	 \return This function indicates if the task was successful, or if an error occurred.
 */
ID_func_vector_state_t rtos_remove_ID_function(ID_function_t ID_func);

/*!
 	 \brief This function changes an ID and its callback function to new ones.

 	 \param[in] ID_func_old ID and callback to be replaced.
 	 \param[in] ID_func_new ID and callback to be set.

 	 \return This function indicates if the task was successful, or if an error occurred.
 */
ID_func_vector_state_t rtos_change_ID_function(ID_function_t ID_func_old, ID_function_t ID_func_new);

/*!
 	 \brief This function returns the number of IDs stored in the ID vector.

 	 \return The number of IDs and callbacks stored in the ID function vector.
 */
uint8_t rtos_get_ID_function_vector_size(void);

/*!
 	 \brief This function turns on the red LED.

 	 \return void.
 */
void turn_on_red_LED(void);

/*!
 	 \brief This function turns on the green LED.

 	 \return void.
 */
void turn_on_green_LED(void);

/*!
 	 \brief THis function turns on the yellow LED.

 	 \return void.
 */
void turn_on_yellow_LED(void);

/*!
 	 \brief This function turns off all LEDs.

 	 \return void.
 */
void turn_off_LEDS(void);

/*!
 	 \brief This function sets the threshold values of the LED. E.g. If the
 	 	 	 threshold value of the red LED is 1500, when the ADC message receives
 	 	 	 2000, the red LED will be turned on.

 	 \note If any variable receives 0, the threshold will not be changed.

 	 \warning Make sure that red > yellow > green, otherwise, the LEDs will malfunction.

 	 \param[in] red New threshold value for the red LED.
 	 \param[in] yellow New threshold value for the yellow LED.
 	 \param[in] green New threshold value for the green LED.

 	 \return void.
 */
void LED_treshold_values(uint16_t red, uint16_t yellow, uint16_t green);

#endif /* RTOS_DRIVER_H_ */
