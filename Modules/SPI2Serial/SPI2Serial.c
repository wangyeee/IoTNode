#include "SPI2Serial.h"
#include "NodeLink.h"

uint8_t cmd_buf[NODE_SIZE + MSG_SIZE];
uint8_t buf_size;

void SPI2Serial_Init(void) {
    for (buf_size = 0; buf_size < NODE_SIZE + MSG_SIZE; buf_size++) {
        cmd_buf[buf_size] = 0;
    }
    buf_size = 0;
}

uint8_t SPI2Serial_Loop(void) {
    /*
     * 0       | 1 ~ 5   | 6    | 7   | 8 ~ 23
     * channel | address | type | seq | payload
     */
    if (buf_size == NODE_SIZE + MSG_SIZE) {
        node_t desc;
        message_t msg;
        uint8_t i;

        desc.channel = cmd_buf[0];
        for (i = 0; i < 5; i++) {
            desc.addr[i] = cmd_buf[i + 1];
        }
        msg.type = cmd_buf[6];
        msg.seq = cmd_buf[7];
        for (i = 0; i < 16; i++) {
            msg.payload[i] = cmd_buf[i + 8];
        }
        nl_send_message(desc, msg);
        buf_size = 0;
        return 1;
    }
    return 0;
}

void USART2_IRQHandler(void) {
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
        if (buf_size < NODE_SIZE + MSG_SIZE) {
            cmd_buf[buf_size++] = (uint8_t) USART_ReceiveData(USART2);
        }
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    } 
}
