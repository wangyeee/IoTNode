#ifndef _PWM_H_
#define _PWM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f0xx.h"
#include "gpio.h"

typedef struct {
    IO_Pin pin;
    TIM_TypeDef* TIMx;
    uint32_t TIM_Clock;
    uint32_t frequency;  /* in Hz */
    uint16_t duty_cycle; /* 0 to 10000 -> 0% to 100% */
    uint8_t tim_chl;
} PWM_CHL;

static PWM_CHL PWM1 = {
    .pin = {
        .RCC_clock = RCC_AHBPeriph_GPIOA,
        .GPIO = GPIOA,
        .GPIO_Pin = GPIO_Pin_9,
        .GPIO_PinSource = GPIO_PinSource9,
        .Mode = GPIO_Mode_AF,
    },
    .TIMx = TIM1,
    .TIM_Clock = RCC_APB2Periph_TIM1,
    .frequency = 50,
    .duty_cycle = 500,
    .tim_chl = 2,
};

void pwm_init(PWM_CHL pwm);
uint16_t pwm_change_duty_cycle(PWM_CHL pwm, uint16_t new_duty_cycle);

#ifdef __cplusplus
}
#endif
#endif /* _PWM_H_ */
