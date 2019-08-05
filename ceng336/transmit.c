#include "common.h"

/**********************************************************************
 * ------------------------------ TRANSMIT -------------------------------
 **********************************************************************/
 
 /**********************************************************************
 * -------------------------- TRANSMIT_TASK ---------------------------
 * 
 * Transmission in certain intervals (here it is 50 ms at the start and
 * 50 ms periodically). It simply waits for an alarm event to be set 
 * (for example a command is ready to be sent), then it simply allows
 * transmission to occur.
 * 
 **********************************************************************/
TASK(TRANSMIT) 
{
    WaitEvent(START_TRANSMIT);
    ClearEvent(START_TRANSMIT);
	SetRelAlarm(ALARM_TSK0, 50, 50);
	while(1) {
        WaitEvent(GO_TRANSMIT);
        ClearEvent(GO_TRANSMIT);
        WaitEvent(ALARM_EVENT);
        ClearEvent(ALARM_EVENT);
        TXSTA1bits.TXEN = 1; //enable transmission.
	}
	TerminateTask();
}
