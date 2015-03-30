#ifndef _NODE_CONFIG_H_
#define _NODE_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "config_defs.h"
#include "stm32f0xx.h"

uint16_t flash_write_buffer(uint8_t* data, uint16_t len);

uint16_t flash_read_buffer(uint8_t* buffer, uint16_t len);


#ifdef __cplusplus
}
#endif
#endif /* _NODE_CONFIG_H_ */
