#include "stdio.h"
#include "stm32f0xx.h"
#include "utility.h"
#include "spi.h"
#include "nrf24l01.h"
#include "node_config.h"
#include "gpio.h"
#include "adc.h"
#include "pwm.h"
#include "ILI9163C.h"
#include "ILI9163C_regs.h"

void NRF24L01_Test_Task(void);
void FLASH_Test_Task(void);

void Test_TFT(void) {
    setRotation(1);
    /* fillScreen(GREEN); */
    fillScreen(WHITE);
    delay_ms(1000);
    fillScreen(BLACK);
    delay_ms(1000);
    fillScreen(WHITE);
    /* fillCircle(50, 50, 25, BLACK); */
    /* fillCircle(50, 100, 25, BLACK); */

    setTextWrap(1);
    setTextSize(1);
    setTextColorBg(BLACK, WHITE);
    setCursor(0, 0);
    /* write_string("Hello World123!", 15); */

    /* for (;;) { */
    /* } */
}

int main(void) {
    uint16_t adc_val;
    float vol;
    //int16_t t;
    init_stdio_USART2();
    init_delay();
    printf("Hello STM32F0!\n");
    /* SPI1_Init(); */
    delay_ms(1000);
    printf("After 1000 ms.\n");
    /* ILI9163C_Init(); */
    /* Test_TFT(); */
    /* pwm_init(PWM1); */
    /* analog_init(ADC_CHL1); */
    //digital_init(PA12);
    //PA15.Mode = GPIO_Mode_IN;
    //digital_init(PA15);
    //FLASH_Test_Task();
    SPI2_Init();
    printf("SPI2 active.\n");
    NRF24L01_Test_Task();

    for (;;) {
    }
}

void NRF24L01_Test_Task(void) {
    uint8_t status;
    uint8_t dt[16];
    uint8_t ret;
/* STC rec test
    uint8_t thisAddr[5]= {53, 69, 149, 231, 231};
    uint8_t remoteAddr[5]= {97, 83, 22, 222, 121};
*/
    uint8_t remoteAddr[5]= {53, 69, 149, 231, 231};
    uint8_t thisAddr[5]= {97, 83, 22, 222, 121};
    
    printf("Init NRF24L01...\n");

    nrf24l01Init();

    delay_ms(100);
    status = nRF24_Check();
    printf("Status=%02x\n", status);
    if (status == 1) {
        printf("nRF24L01 probe failed!\n");
        for (;;);
    }

    printf("Sending data...\n"); // working...
    /* nRF24_TXMode(); */
    /* nrfSetRxMode(96, 5, thisAddr); */
    /*
    for (;;) {
        if (nrfDataReady()) {
            char disp[6];
            ret = nrfGetReceivedData(dt, 16);
            printf("NRF status:%d, data:\n", ret);
            for (status = 0; status < 16; status++) {
                uint16_t d;
                d = 0x0000 | dt[status];
                printf("0x%02X ", d);
            }
            printf("\n");
        }
    }*/
    for (;;) {
        uint i;
        for (i = 0; i < 16; i++) {
            dt[i] = i + 'a';
        }
        /* ret = nRF24_TXPacket(dt, 16); */
        ret = nrfSendData(96, 5, remoteAddr, 16, dt);

        printf("NRF sending data = %c, ret = %d\n", dt[0], ret);
        delay_ms(500);
        /* nRF24_TXMode(); */
    }
    /* for (;;) { */
    /*     for (dt = 'a'; dt <= 'z'; dt++) { */
    /*         // sending 'x' periodically... */
    /*         printf("Sending data: %c\n", dt); */
    /*         ret = nrfSendData(92, 5, remoteAddr, 1, &dt); */
    /*         printf("NRF ret = %d\n", ret); */
    /*         delay_ms(500); */
    /*     } */
    /* } */
}

void FLASH_Test_Task(void) {
    uint16_t i;
    uint8_t data[4];
    uint8_t buffer[4];

    data[0] = (uint8_t) 'A';
    data[1] = (uint8_t) 'B';
    data[2] = (uint8_t) 'C';
    data[3] = (uint8_t) 0;

    flash_read_buffer(buffer, 4);
    printf("Header = %s\n", data);
    printf("Flash = %c%c%c\n", buffer[0], buffer[1], buffer[2]);

    for (i = 0; i < 3; i++) {
        printf("d 0x%02x =?= b 0x%02x\n", data[i], buffer[i]);
        if (data[i] != buffer[i]) {
            i = 0;
            break;
        }
    }
    printf("d 0x%02x =?= b 0x%02x\n", data[3], buffer[3]);
    if (i == 0) {
        printf("data not saved!\ninitializing data storage.\n");
        for (i = 0; i < 4; i++) {
            buffer[i] = data[i];
        }

        i = flash_write_buffer(buffer, 4);
        if (i == 4) {
            printf("Data initialized successfully!\n");
        } else {
            printf("Data initialization error: %d\n", i);
        }
    } else {
        printf("Data loaded, version: %d.\n", buffer[3]);
        buffer[3] = buffer[3] + 1;
        i = flash_write_buffer(buffer, 4);
        if (i == 4) {
            printf("Data updated successfully!\n");
        } else {
            printf("Data update error: %d\n", i);
        }
    }
}
