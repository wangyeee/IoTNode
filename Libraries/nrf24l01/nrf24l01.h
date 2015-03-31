#ifndef _NRF24L01_H_
#define _NRF24L01_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f0xx.h"
#include "nrf24l01_regs.h"

//#define RECEIVE_DATA_WIDTH 4
//#define SEND_MAX_RETRY 5

#define nRF24_RX_ADDR_WIDTH 5  // nRF24 RX address width
#define nRF24_TX_ADDR_WIDTH 5  // nRF24 TX address width

#define NRF_CS_RCC_CLOCK RCC_AHBPeriph_GPIOB
#define NRF_CS_GPIO GPIOB
#define NRF_CS_PIN GPIO_Pin_2

#define NRF_CE_RCC_CLOCK RCC_AHBPeriph_GPIOB
#define NRF_CE_GPIO GPIOB
#define NRF_CE_PIN GPIO_Pin_12

#define NRF_INT_RCC_CLOCK RCC_AHBPeriph_GPIOA
#define NRF_INT_GPIO GPIOA
#define NRF_INT_PIN GPIO_Pin_8

void nRF24_init();

uint8_t nRF24_RWReg(uint8_t reg, uint8_t value);
uint8_t nRF24_ReadReg(uint8_t reg);
uint8_t nRF24_ReadBuf(uint8_t reg, uint8_t *pBuf, uint8_t count);
uint8_t nRF24_WriteBuf(uint8_t reg, uint8_t *pBuf, uint8_t count);

uint8_t nRF24_Check(void);

void nRF24_RXMode(uint8_t RX_PAYLOAD);
void nRF24_TXMode(void);
uint8_t nRF24_DataReady(void);
uint8_t nRF24_TXPacket(uint8_t* pBuf, uint8_t TX_PAYLOAD);
uint8_t nRF24_RXPacket(uint8_t* pBuf, uint8_t RX_PAYLOAD);
void nRF24_ClearIRQFlags(void);

// TODO go through function list and refactor
/*
void nrf24l01Init(void);
uint8_t nrfSendData(uint8_t rfChannel, uint8_t addrLen, uint8_t *txAddr, uint8_t dataLen, uint8_t *txData);
void nrfSetRxMode(uint8_t rfChannel, uint8_t addrWidth, uint8_t *rxAddr);
uint8_t nrfCheckACK(void);
uint8_t nrfGetReceivedData(uint8_t *dataBuffer, uint8_t len);
uint8_t NRF24L01_Read_Status(void);
uint8_t nRF24_Check(void);
*/
#ifdef __cplusplus
}
#endif

#endif /* _NRF24L01_H_ */
