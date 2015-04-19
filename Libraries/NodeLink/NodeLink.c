#include "NodeLink.h"
#include "nrf24l01.h"

static message_listener listener_table[MSG_LISTENER_AMOUNT];

uint8_t nl_send_message(node_t dest, message_t msg) {
    return nrfSendData(dest.channel, NRF_ADDR_LEN, dest.addr, MSG_SIZE, (uint8_t*) &msg);
}

uint8_t nl_register_message_listener(message_listener listener, msg_type type, uint8_t pri) {
    listener_table[type] = listener;
    return 0;
}

uint8_t default_message_listener(node_t from, uint8_t* msg, uint8_t len) {
    // NOP
    return 0;
}

void nl_incoming_message(message_t* msg) {
    if (msg->type >= HEART_BEAT && msg->type < MSG_LISTENER_AMOUNT) {
        node_t t;
        // TODO ...
        listener_table[msg->type](t, msg->payload, MSG_PAYLOAD_SIZE);
    }
}

void init_node_link(void) {
    uint8_t i;

    for (i = 0; i < MSG_LISTENER_AMOUNT; i++) {
        nl_register_message_listener(default_message_listener, i, 0);
    }
}
