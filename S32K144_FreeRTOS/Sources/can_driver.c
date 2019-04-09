/*!
 	 \file can_driver.c

 	 \brief This is the source file of the CAN device driver for
 	 	 	 the S32K144EVB development board. All the initialization
 	 	 	 and control functions are found in this source.

 	 \author HEMI team
 	 	 	 Arpio Fernandez, Leon 				ie702086@iteso.mx
 	 	 	 Barragan Alvarez, Daniel 			ie702554@iteso.mx
 	 	 	 Delsordo Bustillo, Jose Ricardo	ie702570@iteso.mx

 	 \date 	27/03/2019
 */

#include "can_driver.h"

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
#define CAN_FD_DISABLE			(0x0003001F)
/** Offset to calculate the msg_size from DLC*/
#define MESSAGE_SIZE_OFF		(0x03)
/** Delay for the Tx*/
#define CAN_DELAY				(10000)

/** Mask to get the MSB of the Rx message*/
#define CAN_RX_MSG_MSB_MASK		(0xFF000000)
/** Mask to enable the interruption of the Rx message buffer*/
#define CAN_SET_RX_BUFF_ISR		(0x10)

/** Size of the variable to concatenate the message received*/
#define TEMP_VAR_SIZE			(2)
/** Defines the bytes in a uint32_t variable*/
#define BYTE_COUNT_4			(4)
/** Shifts necessary to shift a byte*/
#define BYTE_SHIFT				(8)
/** Offset of the message position in the MB*/
#define MSG_POS_OFFSET			(3)
/** Shifts necessary to shift from a MSB to a LSB in a uint32_t*/
#define MSB_TO_LSB_SHIFT		(24)
/** Position of the low byte in the temp array*/
#define LOW_BYTE_TEMP			(0)
/** Position of the high byte in the temp array*/
#define HIGH_BYTE_TEMP			(1)
/** Offset of 1 for an array position*/
#define ARRAY_OFFSET_1			(1)

/** Maximum DLC that can be sent*/
#define MAX_DLC					(8)

/** Variable to store the code of the Rx MB*/
static uint32_t RxCODE;
/** Variable to store the ID of the Rx MB*/
static uint32_t RxID;
/** Variable to store the DLC of the Rx MB*/
static uint32_t RxLENGTH;

/** This function initializes the CAN*/
void CAN_Init(can_init_config_t can_init)
{
	/** Counter to clean the RAM*/
	uint8_t counter;

	/** For CAN0*/
	if(CAN0 == can_init.base)
	{
		/** Enables the peripheral clock*/
		PCC->PCCn[PCC_FlexCAN0_INDEX] |= PCC_PCCn_CGC_MASK;
	}

	/** For CAN1*/
	else if(CAN1 == can_init.base)
	{
		/** Enables the peripheral clock*/
		PCC->PCCn[PCC_FlexCAN1_INDEX] |= PCC_PCCn_CGC_MASK;
	}

	/** For CAN2*/
	else if(CAN2 == can_init.base)
	{
		PCC->PCCn[PCC_FlexCAN2_INDEX] |= PCC_PCCn_CGC_MASK;
	}

	/** Disables the module*/
	can_init.base->MCR |= CAN_MCR_MDIS_MASK;
	/** Sets the clock source to the oscillator clock*/
	can_init.base->CTRL1 &= (~CAN_CTRL1_CLKSRC_MASK);
	/** Enables the module*/
	can_init.base->MCR &= (~CAN_MCR_MDIS_MASK);

	/** Waits for the module to enter freeze mode, to manage the CTRL and other registers*/
	while(!((can_init.base->MCR & CAN_MCR_FRZACK_MASK) >> CAN_MCR_FRZACK_SHIFT));

	/** Configures the speed, and other parameters*/
	can_init.base->CTRL1 = can_init.speed;

	/** Initializes the MB RAM in 0*/
	for(counter = INIT_VAL ; MAX_MSG_BUFFERS > counter ; counter ++)
	{
		can_init.base->RAMn[counter] = INIT_VAL;

		/** Sets the ID masks to not check the ID*/
		if(MAX_FILTER_BUFFERS > counter)
		{
			can_init.base->RXIMR[counter] = NOT_CHECK_ANY_ID;
		}
	}

	/** Sets the global ID mask to not check any ID*/
	can_init.base->RXMGMASK = NOT_CHECK_ANY_ID;

	/** Enables the MB 4 for reception*/
	can_init.base->RAMn[(RX_BUFF_OFFSET * MSG_BUF_SIZE) + CODE_AND_DLC_POS] = ENABLE_RX_BUFF;

	/** CAN FD not used*/
	can_init.base->MCR = CAN_FD_DISABLE;

	/** Waits for the module to exit freeze mode*/
	while ((can_init.base->MCR && CAN_MCR_FRZACK_MASK) >> CAN_MCR_FRZACK_SHIFT);
	/** Waits for the module to be ready*/
	while ((can_init.base->MCR && CAN_MCR_NOTRDY_MASK) >> CAN_MCR_NOTRDY_SHIFT);
}

/** This function enables the interruption for the Rx message buffer*/
void CAN_enable_rx_interruption(CAN_Type* base)
{
	base->IMASK1 = CAN_SET_RX_BUFF_ISR;
}

/** This function sends a message via CAN*/
void CAN_send_message(can_message_tx_config_t can_message_tx)
{
	/** Counter to set the message to the MB*/
	uint16_t counter = INIT_VAL;
	uint16_t byte_counter = INIT_VAL;
	uint32_t temp[TEMP_VAR_SIZE] = {INIT_VAL};

	/** Standard ID can only be of 11 bits*/
	can_message_tx.ID &= STD_ID_MASK;

	if(MAX_DLC < can_message_tx.DLC)
	{
		can_message_tx.DLC = MAX_DLC;
	}

	/** Clears CAN 0 MB 0 interruption flag*/
	can_message_tx.base->IFLAG1 = CLEAR_MB_0;

	/** Sets the message in the CAN tx buffer*/
	for(counter = INIT_VAL ; counter < can_message_tx.DLC ; counter ++)
	{
		/** Concatenates each of the bytes in msg to temp*/
		temp[(counter / BYTE_COUNT_4)] |= (uint32_t)((*can_message_tx.msg) << ((MSG_POS_OFFSET - byte_counter) * BYTE_SHIFT));

		/** Increments the pointer to msg*/
		can_message_tx.msg ++;
		/** Increments the number of bytes concatenated*/
		byte_counter ++;

		/** When first 4 bytes of temp are full*/
		if(BYTE_COUNT_4 == byte_counter)
		{
			/** Resets the bytes concatenated count*/
			byte_counter = INIT_VAL;
		}
	}

	/** Sets the temp variable to the MB*/
	can_message_tx.base->RAMn[(TX_BUFF_OFFSET * MSG_BUF_SIZE) + MSG_POS] = temp[LOW_BYTE_TEMP];
	can_message_tx.base->RAMn[(TX_BUFF_OFFSET * MSG_BUF_SIZE) + ARRAY_OFFSET_1 + MSG_POS] = temp[HIGH_BYTE_TEMP];

	/** Sets the ID to the bits 28-18 (ID bits for standard format)*/
	can_message_tx.base->RAMn[(TX_BUFF_OFFSET * MSG_BUF_SIZE) + ID_POS] = (can_message_tx.ID << STD_ID_SHIFT);

	/** Sets the DLC and the CAN command to transmit*/
	can_message_tx.base->RAMn[(TX_BUFF_OFFSET * MSG_BUF_SIZE) + CODE_AND_DLC_POS] = (can_message_tx.DLC << CAN_WMBn_CS_DLC_SHIFT) | TX_BUFF_TRANSMITT;

	while(!CAN_get_tx_status(CAN0));
	can_message_tx.base->IFLAG1 = CLEAR_MB_0;
}

/** This function receives a message from CAN*/
void CAN_receive_message(can_message_rx_config_t *can_message_rx)
{
	/** Counter to get the message*/
	uint8_t counter = INIT_VAL;

	/** Gets the rx code*/
	RxCODE = ((*can_message_rx).base->RAMn[(RX_BUFF_OFFSET * MSG_BUF_SIZE) + CODE_AND_DLC_POS] & CAN_CODE_MASK) >> CAN_CODE_SHIFT;
	/** Gets ID*/
	RxID = ((*can_message_rx).base->RAMn[(RX_BUFF_OFFSET * MSG_BUF_SIZE) + ID_POS] & CAN_WMBn_ID_ID_MASK) >> STD_ID_SHIFT;
	/** Gets the DLC*/
	RxLENGTH = ((*can_message_rx).base->RAMn[(RX_BUFF_OFFSET * MSG_BUF_SIZE) + CODE_AND_DLC_POS] & CAN_WMBn_CS_DLC_MASK);
	RxLENGTH  >>= CAN_WMBn_CS_DLC_SHIFT;

	/** Gets each of the bytes*/
	for(counter = INIT_VAL ; counter < RxLENGTH ; counter ++)
	{
		/** Gets the highest byte of the MB and sets it to msg*/
		((*can_message_rx).msg[counter]) = (uint8_t)((((*can_message_rx).base->RAMn[(RX_BUFF_OFFSET * MSG_BUF_SIZE) +
													(counter / BYTE_COUNT_4) + MSG_POS]) & CAN_RX_MSG_MSB_MASK) >> MSB_TO_LSB_SHIFT);

		/** Shifts the remaining message to the left*/
		(*can_message_rx).base->RAMn[(RX_BUFF_OFFSET * MSG_BUF_SIZE) + (counter / BYTE_COUNT_4) + MSG_POS] <<= BYTE_SHIFT;
	}

	/** Clears the reception flag*/
	(*can_message_rx).base->IFLAG1 = CLEAR_MB_4;

	/** Returns the data*/
	((*can_message_rx).ID) = (uint16_t)RxID;
	/** Sets the DLC*/
	((*can_message_rx).DLC) = (uint8_t)(RxLENGTH);

	/** Sets the MB ready for another message*/
	(*can_message_rx).base->RAMn[(RX_BUFF_OFFSET * MSG_BUF_SIZE) + CODE_AND_DLC_POS] |= ENABLE_RX_BUFF;
}

/** Gets the flag of the RX buffer*/
CAN_rx_status_t CAN_get_rx_status(CAN_Type* base)
{
	return ((CAN_rx_status_t)((base->IFLAG1 >> RX_MB_FLAG_SHIFT) & BIT_MASK));
}

/** Gets the flag of the RX buffer*/
CAN_tx_status_t CAN_get_tx_status(CAN_Type* base)
{
	return((CAN_tx_status_t)(base->IFLAG1 & BIT_MASK));
}

/** This function clears the RX and TX buffer flags*/
void CAN_clear_tx_and_rx_flags(CAN_Type* base)
{
	base->IFLAG1 = CLEAR_ALL_FLAGS;
}
