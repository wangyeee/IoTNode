#ifndef _NODE_LINK_H_
#define _NODE_LINK_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef STM32F0XX
#include "stm32f0xx.h"
#endif

#define MSG_PAYLOAD_SIZE 16

typedef enum {
    HEART_BEAT = 0,
    RTC_SYNC,
    SWITCH,
    // etc.
    MSG_LISTENER_AMOUNT
} msg_type;

typedef enum {
    DISABLED = 0,
    ENABLED
} periph_cfg;

typedef struct {
    uint8_t channel;
    uint8_t addr[5];
} node_t;

typedef struct {
    uint8_t type;
    uint8_t seq;
    uint8_t payload[MSG_PAYLOAD_SIZE];
} message_t;

#define NODE_SIZE sizeof(node_t)
#define MSG_SIZE sizeof(message_t)

typedef uint8_t (*message_listener)(node_t from, uint8_t* msg, uint8_t len);

void init_node_link(void);

uint8_t nl_send_message(node_t dest, message_t msg);

uint8_t nl_register_message_listener(message_listener listener, msg_type type, uint8_t pri);

void nl_incoming_message(message_t* msg);

#ifdef __cplusplus
}
#endif
#endif /* _NODE_LINK_H_ */
