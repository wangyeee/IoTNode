#include "stdio.h"
#include "stm32f0xx.h"
#include "utility.h"
#include "spi.h"
#include "nrf24l01.h"
#include "node_config.h"
#include "gpio.h"
#include "adc.h"
#include "pwm.h"
#include "SPI2Serial.h"
#include "NodeLink.h"

void spi2serial_main(void) {
    uint8_t status;
    uint8_t thisAddr[5]= {97, 83, 22, 222, 121};

    init_stdio_USART2();
    init_delay();

    SPI2_Init();

    delay_ms(10);

    nrf24l01Init();

    delay_ms(100);
    status = nRF24_Check();

    if (status == 1) {
        for (;;);
    }
    nrfSetRxMode(92, 5, thisAddr);

    for (;;) {
        status = SPI2Serial_Loop();
        if (status) {
            nrfSetRxMode(92, 5, thisAddr);
        }
    }
}

typedef struct {
    uint8_t id;
    uint8_t enabled;
    uint8_t status;
    DIO_Pin pin;
} switch_t;

switch_t led_pa12;

uint8_t switch_message_listener(node_t from, uint8_t* msg, uint8_t len) {
    if (msg[0]) {
        led_pa12.status = 1;
    } else {
        led_pa12.status = 0;
    }
    return 0;
}

void node_main(void) {
    uint8_t status;
    uint8_t thisAddr[5]= {97, 89, 64, 222, 121};

    init_stdio_USART2();
    init_delay();
    led_pa12.id = 0;
    led_pa12.enabled = 1;
    led_pa12.status = 0;
    led_pa12.pin = PA12;

    if (led_pa12.enabled) {
        digital_init(led_pa12.pin);
        {
            digital_high(led_pa12.pin);
            delay_ms(500);
            digital_low(led_pa12.pin);
            delay_ms(500);
            digital_high(led_pa12.pin);
            delay_ms(500);
            digital_low(led_pa12.pin);
            delay_ms(500);
            digital_high(led_pa12.pin);
            delay_ms(500);
        }
        if (led_pa12.status) {
            digital_high(led_pa12.pin);
        } else {
            digital_low(led_pa12.pin);
        }
    }

    SPI2_Init();

    delay_ms(10);

    nrf24l01Init();

    delay_ms(100);
    status = nRF24_Check();

    if (status == 1) {
        for (;;);
    }
    nl_register_message_listener(switch_message_listener, SWITCH, 0);
    nrfSetRxMode(92, 5, thisAddr);

    for (;;) {
        if (led_pa12.enabled) {
            if (led_pa12.status) {
                digital_high(led_pa12.pin);
            } else {
                digital_low(led_pa12.pin);
            }
        }
    }
}

__attribute__((noreturn))
int main(void) {
    node_main();
    /* spi2serial_main(); */
    for (;;);
}
