#ifndef _SPI_H_
#define _SPI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f0xx.h"

void SPI2_Init(void);
uint8_t SPI2_Transfer(uint8_t data);

#ifdef __cplusplus
}
#endif
#endif /* _SPI_H_ */
