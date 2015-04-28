#ifndef _SWITCHCONTROL_H_
#define _SWITCHCONTROL_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef STM32F0XX
#include "stm32f0xx.h"
#include "gpio.h"
#endif

#include "NodeLink.h"

/*
 * id  GPIO
 *  0  PA0
 *  1  PA1
 *  2  PA12
 *  3  PA15
 *  4  PC13
 *  5  PF6
 *  6  PF7
 */

typedef struct {
    uint8_t id : 4;
    uint8_t status : 2;
    uint8_t enabled : 2;
#ifdef STM32F0XX
/* Compile for nodes */
    DIO_Pin pin;
#else
/* Compile for server */
    node_t node;
#endif
} switch_t;

#define SWITCH_SIZE sizeof(switch_t)

typedef enum {
    ON = 0,
    OFF
} switch_status;

#ifdef STM32F0XX

void init_switchs(void);
uint8_t switch_message_listener(node_t from, uint8_t* msg, uint8_t len);

#endif /* STM32F0XX */

#ifdef __cplusplus
}
#endif
#endif /* _SWITCHCONTROL_H_ */
