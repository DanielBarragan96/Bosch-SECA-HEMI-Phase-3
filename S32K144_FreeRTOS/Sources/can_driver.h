/*!
 	 \file can_driver.h

 	 \brief This is the header file of the CAN device driver for
 	 	 	 the S32K144EVB development board. All the initialization
 	 	 	 and control functions are found in this header.

 	 \author HEMI team
 	 	 	 Arpio Fernandez, Leon 				ie702086@iteso.mx
 	 	 	 Barragan Alvarez, Daniel 			ie702554@iteso.mx
 	 	 	 Delsordo Bustillo, Jose Ricardo	ie702570@iteso.mx

 	 \date 	27/03/2019
 */

#ifndef CAN_DRIVER_H_
#define CAN_DRIVER_H_

#include "S32K144.h"

/** Defines the speed of 500 Kbps*/
#define CAN_CTRL1_SPEED_500KBPS			(0x00DB0006)
/** Defines the speed of 250 Kbps*/
#define CAN_CTRL1_SPEED_250KBPS			(0x01DB0006)
/** Defines the speed of 100 Kbps*/
#define CAN_CTRL1_SPEED_100KBPS			(0x04DB0006)
/** Defines the speed of 50 Kbps*/
#define CAN_CTRL1_SPEED_50KBPS			(0x09DB0006)

/*!
 	 \brief Enumerator to define whether the rx buffer has interrupted
 	 	 	 or not.
 */
typedef enum
{
	rx_not_interrupted,	/*!< Rx message buffer has not interrupted*/
	rx_interrupted		/*!< Rx message buffer interrupted*/
}CAN_rx_status_t;

/*!
 	 \brief Enumerator to define whether the tx buffer has interrupted
 	 	 	 or not
 */
typedef enum
{
	tx_not_interrupted,	/*!< Tx message buffer has not interrupted*/
	tx_interrupted		/*!< Tx message buffer interrupted*/
}CAN_tx_status_t;

/*!
 	 \brief Arguments to initialize CAN (RTOS)
 */
typedef struct
{
	CAN_Type* base; /*!< CAN to be initialized*/
	uint32_t speed;	/*!< CAN speed to be set*/
}can_init_config_t;

/*!
 	 \brief Arguments to handle tx messages of CAN (RTOS)
 */
typedef struct
{
	CAN_Type* base;	/*!< CAN from which the message will be sent from*/
	uint16_t ID;	/*!< ID of the message to be sent*/
	uint8_t* msg;	/*!< Message to be sent*/
	uint8_t DLC;	/*!< DLC of the message to be sent*/
}can_message_tx_config_t;

/*!
 	 \brief Arguments to handle tx messages of CAN (RTOS)
 */
typedef struct
{
	CAN_Type* base;	/*!< CAN which will receive the message*/
	uint16_t ID;	/*!< ID received*/
	uint8_t msg[8];	/*!< Message received*/
	uint8_t DLC;	/*!< DLC received*/
}can_message_rx_config_t;

/*!
 	 \brief This function initializes the CAN module.

 	 \param[in] can_init Configuration for the CAN driver.

 	 \return void.
 */
void CAN_Init(can_init_config_t can_init);

/*!
 	 \brief This function enables the interruption for the Rx MB.

 	 \param[in] base CAN whose interruption will be enabled.

 	 \return void.
 */
void CAN_enable_rx_interruption(CAN_Type* base);

/*!
 	 \brief This function sends a message via CAN using the standard ID.

 	 \note If the DLC is higher than 8, it will be set to 8.

	 \param[in] can_message_tx Message structure to be sent.

 	 \return void.
 */
void CAN_send_message(can_message_tx_config_t can_message_tx);

/*!
 	 \brief This function reads a message received via CAN.

 	 \note First make sure the CAN has received a message using the function CAN_get_rx_status().
 	 \note This function erases the interruption flag of the Rx buffer.

	 \param[out] can_message_rx Message structure with the data received.

 	 \return void.
 */
void CAN_receive_message(can_message_rx_config_t *can_message_rx);

/*!
 	 \brief This function gets the status of the Rx message buffer.

 	 \param[in] base CAN module from which the Rx status will be checked.

 	 \return Whether the CAN module has received a message or not.
 */
CAN_rx_status_t CAN_get_rx_status(CAN_Type* base);

/*!
 	 \brief This function gets the status of the Tx message buffer.

 	 \param[in] base CAN module from which the Tx status will be checked.

 	 \return Whether the CAN module has finished transmitting a message or not.
 */
CAN_tx_status_t CAN_get_tx_status(CAN_Type* base);


/*!
 	 \brief This function erases the Tx and Rx buffer flags.

 	 \param[in] base CAN module whose flags will be erased.

 	 \return void.
 */
void CAN_clear_tx_and_rx_flags(CAN_Type* base);

#endif /* CAN_DRIVER_H_ */
