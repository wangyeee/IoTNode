#ifndef _GPIO_H_
#define _GPIO_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f0xx.h"

typedef struct {
    uint32_t RCC_clock;
    GPIO_TypeDef* GPIO;
    uint32_t GPIO_Pin;
    GPIOMode_TypeDef Mode;
} IO_Pin;

#define DIO_Pin IO_Pin

static DIO_Pin PA12 = {
    .RCC_clock = RCC_AHBPeriph_GPIOA,
    .GPIO = GPIOA,
    .GPIO_Pin = GPIO_Pin_12,
    .Mode = GPIO_Mode_OUT,
};

static DIO_Pin PA15 = {
    .RCC_clock = RCC_AHBPeriph_GPIOA,
    .GPIO = GPIOA,
    .GPIO_Pin = GPIO_Pin_15,
    .Mode = GPIO_Mode_OUT,
};

void digital_init(DIO_Pin pin);
void digital_high(DIO_Pin pin);
void digital_low(DIO_Pin pin);
uint8_t digital_read(DIO_Pin pin);

#ifdef __cplusplus
}
#endif
#endif /* _GPIO_H_ */
