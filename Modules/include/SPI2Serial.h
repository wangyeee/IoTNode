#ifndef _SPI2SERIAL_H_
#define _SPI2SERIAL_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef STM32F0XX
#include "stm32f0xx.h"
#endif

void SPI2Serial_Init(void);

uint8_t SPI2Serial_Loop(void);

#ifdef __cplusplus
}
#endif
#endif /* _SPI2SERIAL_H_ */
