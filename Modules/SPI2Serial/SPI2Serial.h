#ifndef _SPI2SERIAL_H_
#define _SPI2SERIAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f0xx.h"

void SPI2Serial_Init(void);

uint8_t SPI2Serial_Loop(void);

#ifdef __cplusplus
}
#endif
#endif /* _SPI2SERIAL_H_ */
