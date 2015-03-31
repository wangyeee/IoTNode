#include "gpio.h"

void digital_init(DIO_Pin pin) {
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_AHBPeriphClockCmd(pin.RCC_clock, ENABLE);
    GPIO_InitStructure.GPIO_Pin = pin.GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode = pin.Mode;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(pin.GPIO, &GPIO_InitStructure);
}

void digital_low(DIO_Pin pin) {
    GPIO_ResetBits(pin.GPIO, pin.GPIO_Pin);
}

void digital_high(DIO_Pin pin) {
    GPIO_SetBits(pin.GPIO, pin.GPIO_Pin);
}

uint8_t digital_read(DIO_Pin pin) {
    if (pin.Mode != GPIO_Mode_IN) {
        // not in digital input mode
        return 255;
    }
    return GPIO_ReadInputDataBit(pin.GPIO, pin.GPIO_Pin);
}
