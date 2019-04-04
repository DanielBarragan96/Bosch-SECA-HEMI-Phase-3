
#ifndef TRANSCEIVER_H_
#define TRANSCEIVER_H_

#include "S32K144.h"

/*********************** NOTE ***************************/
/** This module is taken from the driver example FlexCAN*/
/********************************************************/

#define SBC_MC33903

void WDOG_disable (void);

void PORT_init (void);

void LPSPI1_init_master(void);

void LPSPI1_init_MC33903(void);

void LPSPI1_transmit_16bits(uint16_t send);

uint16_t LPSPI1_receive_16bits (void);

#endif /* TRANSCEIVER_H_ */
