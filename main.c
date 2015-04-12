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
    setCursor(10, 15);
    write_string("Hello World123!", 15);

    for (;;) {
    }
}

int main(void) {
    uint16_t adc_val;
    float vol;
    //int16_t t;
    init_stdio_USART2();
    init_delay();
    printf("Hello STM32F0!\n");
    SPI1_Init();
    delay_ms(1000);
    printf("After 1000 ms.\n");
    ILI9163C_Init();
    Test_TFT();
    /* pwm_init(PWM1); */
    /* analog_init(ADC_CHL1); */
    //digital_init(PA12);
    //PA15.Mode = GPIO_Mode_IN;
    //digital_init(PA15);
    //FLASH_Test_Task();
    //SPI2_Init();
    //printf("SPI2 active.\n");
    /* NRF24L01_Test_Task(); */
    for (;;) {
        /* clearScreen(WHITE); */
        /* printf("white.\n"); */
        /* delay_ms(500); */
        /* clearScreen(BLACK); */
        /* printf("black.\n"); */
        /* delay_ms(500); */
        /*
          for (t = 0; t <= 9000; t+=50) {
          printf("Change duty cycle to %d\n", t);
          pwm_change_duty_cycle(PWM1, t);
          delay_ms(20);
          }
          for (t = 9000; t >= 0; t-=50) {
          printf("Change duty cycle to %d\n", t);
          pwm_change_duty_cycle(PWM1, t);
          delay_ms(20);
          }
        */
        /* adc_val = analog_read(ADC_CHL1); */
        /* vol = adc_val / 4096.0f * 3.3f; */
        /* adc_val += adc_val; */
        /* printf("ADC1 value = %d, voltage = %d.%02dV\n", adc_val, (int) vol, (int)((vol - (int)vol) * 100)); */
        /* printf("Change duty cycle to %d\n", adc_val); */
        /* pwm_change_duty_cycle(PWM1, adc_val); */
        //digital_low(PA12);
        //sw = digital_read(PA15);
        //if (sw) {
        //    digital_high(PA12);
        //}
        //delay_ms(500);
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
    /* uint8_t thisAddr[5]= {97, 83, 22, 222, 111}; */
    /* uint8_t remoteAddr[5]= {97, 83, 22, 222, 121}; */

    printf("Init NRF24L01...\n");

    nRF24_init();

    delay_ms(100);
    status = nRF24_Check();
    printf("Status=%02x\n", status);
    if (status == 1) {
        printf("nRF24L01 probe failed!\n");
        for (;;);
    }

    printf("Receiving data...\n"); // working...
    nRF24_TXMode();
    nRF24_ClearIRQFlags();

    /* nrfSetRxMode(92, 5, thisAddr); //接收92频道，5字节地址 */

    for (;;) {
        ret = nRF24_RXPacket(&dt, 1);

        printf("NRF data = %c, ret = %d\n", dt, ret);
        nRF24_RXMode(1);

        delay_ms(500);
    }
    /*
      for (;;) {
      for (dt = 'a'; dt <= 'z'; dt++) {
      ret = nRF24_TXPacket(&dt, 1);
      printf("NRF sending data = %c, ret = %d\n", dt, ret);
      delay_ms(500);
      nRF24_TXMode();
      }
      }
    */
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
