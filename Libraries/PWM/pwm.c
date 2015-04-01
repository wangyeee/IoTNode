#include "pwm.h"

void pwm_init(PWM_CHL pwm) {
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_OCInitTypeDef TIM_OCInitStructure;
    uint16_t TimerPeriod;
    uint16_t ChannelPulse;

    digital_init(pwm.pin);
    GPIO_PinAFConfig(pwm.pin.GPIO, pwm.pin.GPIO_PinSource, GPIO_AF_2);
    TimerPeriod = (SystemCoreClock / pwm.frequency ) - 1;
    ChannelPulse = (uint16_t) (((uint32_t) pwm.duty_cycle * (TimerPeriod - 1)) / 10000);

    if (pwm.TIM_Clock == RCC_APB1Periph_TIM3) {
        RCC_APB1PeriphClockCmd(pwm.TIM_Clock, ENABLE);
    } else {
        RCC_APB2PeriphClockCmd(pwm.TIM_Clock, ENABLE);
    }

    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period = TimerPeriod;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;

    TIM_TimeBaseInit(pwm.TIMx, &TIM_TimeBaseStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_Low;
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
    TIM_OCInitStructure.TIM_Pulse = ChannelPulse;

    switch (pwm.tim_chl) {
    case 1:
        TIM_OC1Init(pwm.TIMx, &TIM_OCInitStructure);
        break;
    case 2:
        TIM_OC2Init(pwm.TIMx, &TIM_OCInitStructure);
        break;
    case 3:
        TIM_OC3Init(pwm.TIMx, &TIM_OCInitStructure);
        break;
    case 4:
        TIM_OC4Init(pwm.TIMx, &TIM_OCInitStructure);
        break;
    }

    TIM_Cmd(pwm.TIMx, ENABLE);
    TIM_CtrlPWMOutputs(pwm.TIMx, ENABLE);
}

uint16_t pwm_change_duty_cycle(PWM_CHL pwm, uint16_t new_duty_cycle) {
    uint16_t old_duty_cycle = pwm.duty_cycle;
    pwm.duty_cycle = new_duty_cycle;
    switch (pwm.tim_chl) {
    case 1:
        TIM_SetCompare1(pwm.TIMx, new_duty_cycle);
        break;
    case 2:
        TIM_SetCompare2(pwm.TIMx, new_duty_cycle);
        break;
    case 3:
        TIM_SetCompare3(pwm.TIMx, new_duty_cycle);
        break;
    case 4:
        TIM_SetCompare4(pwm.TIMx, new_duty_cycle);
        break;
    }
    return old_duty_cycle;
}
