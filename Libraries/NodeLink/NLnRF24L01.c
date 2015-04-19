#include "NodeLink.h"
#include "nrf24l01.h"
#include "spi.h"

static uint8_t data_buffer[MSG_SIZE];

void EXTI4_15_IRQHandler(void) {
    if (EXTI_GetITStatus(EXTI_Line8) != RESET) {
        uint8_t status;

        status = nrfReadReg(NRF24L01_R_REGISTER | NRF24L01_STATUS);
        if (status & 0x40) {
            nrfReadRxData(NRF24L01_R_RX_PAYLOAD, data_buffer, MSG_SIZE);
            nrfWriteReg(NRF24L01_W_REGISTER | NRF24L01_STATUS, 0xff);
            NRF_CS_LOW;
            SPI2_Transfer(NRF24L01_FLUSH_RX);
            NRF_CS_HIGH;

            nl_incoming_message((message_t*) data_buffer);
        }
        EXTI_ClearITPendingBit(EXTI_Line8);
    }
}
