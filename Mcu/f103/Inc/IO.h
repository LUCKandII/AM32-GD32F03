/**
 * IO.h
 *
 * Input/Output configuration for DShot input capture
 */

#ifndef IO_H
#define IO_H

#include "main.h"

void IO_Init(void);
uint8_t getInputPinState(void);
void receiveDshotDma(void);
void sendDshotDma(void);
void setInputPullDown(void);
void setInputPullUp(void);
void setInputPullNone(void);
void setInputPolarityRising(void);
void enableHalfTransferInt(void);

#endif /* IO_H */
