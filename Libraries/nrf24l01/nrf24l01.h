#ifndef _NRF24L01_H_
#define _NRF24L01_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f0xx.h"
#include "nrf24l01_regs.h"

#define NRF_ADDR_LEN 5

#define RECEIVE_DATA_WIDTH 16
#define SEND_MAX_RETRY 5

#define NRF_CS_RCC_CLOCK RCC_AHBPeriph_GPIOB
#define NRF_CS_GPIO GPIOB
#define NRF_CS_PIN GPIO_Pin_2

#define NRF_CE_RCC_CLOCK RCC_AHBPeriph_GPIOB
#define NRF_CE_GPIO GPIOB
#define NRF_CE_PIN GPIO_Pin_12

#define NRF_INT_RCC_CLOCK RCC_AHBPeriph_GPIOA
#define NRF_INT_GPIO GPIOA
#define NRF_INT_PIN GPIO_Pin_8

#define NRF_CS_HIGH GPIO_SetBits(NRF_CS_GPIO, NRF_CS_PIN)
#define NRF_CS_LOW GPIO_ResetBits(NRF_CS_GPIO, NRF_CS_PIN)
#define NRF_CE_HIGH GPIO_SetBits(NRF_CE_GPIO, NRF_CE_PIN)
#define NRF_CE_LOW GPIO_ResetBits(NRF_CE_GPIO, NRF_CE_PIN)

void nrf24l01Init(void);
uint8_t nrfSendData(uint8_t rfChannel, uint8_t addrLen, uint8_t *txAddr, uint8_t dataLen, uint8_t *txData);
void nrfSetRxMode(uint8_t rfChannel, uint8_t addrWidth, uint8_t *rxAddr);
uint8_t nrfCheckACK(void);
/* uint8_t nrfGetReceivedData(uint8_t *dataBuffer, uint8_t len); */
uint8_t NRF24L01_Read_Status(void);
uint8_t nRF24_Check(void);

/* uint8_t nrfDataReady(void); */

uint8_t nrfReadReg(uint8_t regAddr);
uint8_t nrfWriteReg(uint8_t regAddr, uint8_t writeData);
uint8_t nrfReadRxData(uint8_t regAddr, uint8_t *rxData, uint8_t dataLen);

#ifdef __cplusplus
}
#endif

#endif /* _NRF24L01_H_ */
