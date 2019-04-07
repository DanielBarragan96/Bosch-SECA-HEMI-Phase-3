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
 	 \brief This function initializes the CAN module.

 	 \param[in] base CAN module to be enabled.
 	 \param[in] speed Speed of the CAN module.

 	 \return void.
 */
void CAN_Init(CAN_Type* base, uint32_t speed);

/*!
 	 \brief This function sends a message via CAN using the standard ID.

 	 \param[in] base CAN module from which the message will be sent.
 	 \param[in] ID ID of the message to be sent.
 	 \param[in] msg Message to be sent.\n
 	 	 	 	 	 This parameter is managed as a uint32_t array, but the message
 	 	 	 	 	 is sent byte by byte.
 	 \param[in] msg_size Length of msg. The maximum size of msg should be 2.
 	 \param[in] DLC Length of the message to be sent in bytes.

 	 \return void.
 */
void CAN_send_message(CAN_Type* base, uint16_t ID, uint8_t* msg, uint8_t DLC);

/*!
 	 \brief This function reads a message received via CAN.

 	 \note First make sure the CAN has received a message using the function CAN_get_rx_status().
 	 \note This function erases the interruption flag of the Rx buffer.

 	 \param[in] base CAN module from which the message will be received.
 	 \param[out] ID ID of the message received.
 	 \param[out] msg Message received.
 	 \param[out] msg_size Length of msg.
 	 \param[out] DLC Length of the received message in bytes.

 	 \return void.
 */
void CAN_receive_message(CAN_Type* base, uint16_t* ID, uint8_t* msg, uint8_t* DLC);

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
