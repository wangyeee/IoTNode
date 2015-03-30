#include "spi.h"

/*
 * PB13 SCK
 * PB14 MISO
 * PB15 MOSI
 */
void SPI2_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef  SPI_InitStructure;

    /* Enable the SPI periph */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

    /* Enable GPIOs clock */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

    /* Configure SPI pins */  
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    /* Connect SCK, MISO and MOSI pins to SPI alternate */  
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource13, GPIO_AF_0);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource14, GPIO_AF_0);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF_0);

    /* Configure SPI */
    SPI_I2S_DeInit(SPI2);
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; 
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;

    /* SPI baudrate is set to 6 MHz maximum (PCLK/SPI_BaudRatePrescaler = 48/8 = 6 MHz) */
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_Init(SPI2, &SPI_InitStructure);

    /* Configure the RX FIFO Threshold */
    SPI_RxFIFOThresholdConfig(SPI2, SPI_RxFIFOThreshold_QF);
    /* Enable SPI */
    SPI_Cmd(SPI2, ENABLE);
}

uint8_t SPI2_Transfer(uint8_t data) {
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
    SPI_SendData8(SPI2, data);
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
    return SPI_ReceiveData8(SPI2);
}
