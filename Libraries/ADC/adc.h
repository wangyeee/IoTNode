#ifndef _ADC_H_
#define _ADC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f0xx.h"
#include "gpio.h"

#define ADC_Pin IO_Pin

typedef struct {
    ADC_Pin pin;
    ADC_TypeDef* ADCx;
    uint32_t ADC_Clock;
} ADC_CHL;

static ADC_CHL ADC_CHL1 = {
    .pin = {
        .RCC_clock = RCC_AHBPeriph_GPIOA,
        .GPIO = GPIOA,
        .GPIO_Pin = GPIO_Pin_4,
        .Mode = GPIO_Mode_AN,
    },
    .ADCx = ADC1,
    .ADC_Clock = RCC_APB2Periph_ADC1,
};

static ADC_CHL ADC_CHL2 = {
    .pin = {
        .RCC_clock = RCC_AHBPeriph_GPIOA,
        .GPIO = GPIOA,
        .GPIO_Pin = GPIO_Pin_5,
        .Mode = GPIO_Mode_AN,
    },
    .ADCx = ADC1,
    .ADC_Clock = RCC_APB2Periph_ADC1,
};

static ADC_CHL ADC_CHL3 = {
    .pin = {
        .RCC_clock = RCC_AHBPeriph_GPIOA,
        .GPIO = GPIOA,
        .GPIO_Pin = GPIO_Pin_6,
        .Mode = GPIO_Mode_AN,
    },
    .ADCx = ADC1,
    .ADC_Clock = RCC_APB2Periph_ADC1,
};

static ADC_CHL ADC_CHL4 = {
    .pin = {
        .RCC_clock = RCC_AHBPeriph_GPIOA,
        .GPIO = GPIOA,
        .GPIO_Pin = GPIO_Pin_7,
        .Mode = GPIO_Mode_AN,
    },
    .ADCx = ADC1,
    .ADC_Clock = RCC_APB2Periph_ADC1,
};

void analog_init(ADC_CHL adc);
uint16_t analog_read(ADC_CHL adc);

#ifdef __cplusplus
}
#endif
#endif /* _ADC_H_ */
