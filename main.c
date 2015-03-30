#include "stdio.h"
#include "stm32f0xx.h"
#include "utility.h"
#include "spi.h"
#include "nrf24l01.h"
#include "node_config.h"

void NRF24L01_Test_Task(void);
void FLASH_Test_Task(void);

int main(void) {
    uint32_t tmp;

    init_stdio_USART2();
    init_delay();
    printf("Hello STM32F0!\n");
    delay_ms(1000);
    printf("After 1000 ms.\n");
    tmp = RCC->CFGR & RCC_CFGR_SWS;

    if (tmp == 0) {
        printf("HSI\n");
    } else if (tmp == 4) {
        printf("HSE\n");
    } else if (tmp == 8) {
        uint32_t pllsource = RCC->CFGR & RCC_CFGR_PLLSRC;
        printf("PLL\n");
        if (pllsource == 0) {
            printf("HSI for PLL.\n");
        } else {
            printf("HSE for PLL: %d.\n", pllsource);
        }
    } else {
        printf("Error:%d, HSI\n", tmp);
    }
    FLASH_Test_Task();
    //SPI2_Init();
    //printf("SPI2 active.\n");
    //NRF24L01_Test_Task();
    for (;;) {
        /* char x; */
        /* for (x = 'a'; x <= 'z'; x++) { */
        /*     printf("Dummy task, char = %c.\n", x); */
        /*     delay_ms(1000); */
        /* } */
    }
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

void NRF24L01_Test_Task(void) {
    uint8_t status;
    uint8_t dt = '\0';
    uint8_t ret;
    uint8_t thisAddr[5]= {97, 83, 22, 222, 111};
    /* uint8_t remoteAddr[5]= {97, 83, 22, 222, 121}; */

    printf("Init NRF24L01...\n");
    nrf24l01Init();

    delay_ms(100);
    status = nRF24_Check();
    printf("Status=%02x\n", status);
    if (status == 1) {
        printf("nRF24L01 probe failed!\n");
        for (;;);
    }

    printf("Receiving data...\n"); // working...
    nrfSetRxMode(92, 5, thisAddr); //接收92频道，5字节地址
    for (;;) {
        ret = nrfGetReceivedData(&dt, 1);
        printf("NRF data = %c, ret = %d\n", dt, ret);
        delay_ms(500);
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
