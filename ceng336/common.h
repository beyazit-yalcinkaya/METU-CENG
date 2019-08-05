#ifndef _COMMON_H
#define COMMON_H

#include "device.h"

/***********************************************************************
 * ------------------------ Timer settings -----------------------------
 **********************************************************************/
#define _10MHZ          63320
#define _16MHZ          61768
#define _20MHZ          60768
#define _32MHZ          57768
#define _40MHZ          55768

/***********************************************************************
 * ----------------------------- Tasks --------------------------------
 **********************************************************************/
#define DATA_RECEIVED    0xA0
#define ALARM_EVENT      0x80
#define GO_TRANSMIT      0x60
#define START_TRANSMIT   0x40
#define TRANSMIT_END     0x20

#define TRANSMIT_ID      1
#define MOVEBOT_ID       2

#define TRANSMIT_PRIO    2
#define MOVEBOT_PRIO     4

#define ALARM_TSK0       0

/**********************************************************************
 * ----------------------- GLOBAL DEFINITIONS -------------------------
 **********************************************************************/

/* System States */
#define _IDLE           1		// waiting state
#define _EXPLORING      2
#define _SOLVING        3


#define _WAITING        5
#define _OPERATING      6


#define SIZE_TM_BUF     32
#define SIZE_RC_BUF     8


/**********************************************************************
 * ----------------------- FUNCTION PROTOTYPES ------------------------
 **********************************************************************/
 /* transmits data using serial communication */
void transmitData();
/* Invoked when receive interrupt occurs; meaning that data is received */
void dataReceived();

void transmitBuffer_push(char *str,unsigned char size,char isMotion);

unsigned char receiveBuffer_pop(char *str);

#endif

/* End of File : common.h */
