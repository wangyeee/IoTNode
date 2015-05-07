#include "NodeLink.h"
#include "nrf24l01.h"
#include "spi.h"

static uint8_t data_buffer[MSG_SIZE];
static uint8_t data_rdy;

uint8_t nRF_Task_Init(void) {
    for (data_rdy = 0; data_rdy < MSG_SIZE; data_rdy++)
        data_buffer[data_rdy] = 0;
    data_rdy = 0;
    return 0;
}

uint8_t nRF_Task_Loop(void) {
    if (data_rdy) {
        nl_incoming_message((message_t*) data_buffer);
        for (data_rdy = 0; data_rdy < MSG_SIZE; data_rdy++)
            data_buffer[data_rdy] = 0;
        data_rdy = 0;
        return 1;  /* something happened... */
    }
    return 0;
}

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
            data_rdy = 1;
        }
        EXTI_ClearITPendingBit(EXTI_Line8);
    }
}
