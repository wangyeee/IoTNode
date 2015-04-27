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
#include "SwitchControl.h"

void spi2serial_main(void) {
    uint8_t status;
    /* TODO load listening address from flash. */
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

void node_main(void) {
    uint8_t status;
    /* TODO load listening address from flash. */
    uint8_t thisAddr[5]= {97, 89, 64, 222, 121};

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
    /* From now on, controls and sensors can be initialized. */
    init_switchs();

    for (;;) {
        /* currently nothing needs to do in the main loop. */
    }
}

__attribute__((noreturn))
int main(void) {
    node_main();
    /* spi2serial_main(); */
    for (;;);
}
