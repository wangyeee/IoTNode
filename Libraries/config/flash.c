#include "node_config.h"
#include "stm32f0xx_flash.h"

#define PAGE_SIZE ((uint32_t)0x0400) /* Page size = 1KByte */
#define FLASH_START_ADDRESS ((uint32_t)0x08002000) /* 8KByte */

// TODO if a SWAP page is needed???

uint16_t flash_write_buffer(uint8_t* data, uint16_t len) {
    uint16_t i;
    uint16_t evil;

    FLASH_Unlock();//unlock flash writing
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPERR);
    FLASH_ErasePage(FLASH_START_ADDRESS);//erase the entire page before you can write as I //mentioned

    // Write one page data
    for (i = 0; i < (len > PAGE_SIZE ? PAGE_SIZE : len); i+=2) {
        evil = (uint16_t) (data[i] << 8) + (uint16_t) data[i + 1];
        FLASH_ProgramHalfWord(FLASH_START_ADDRESS + i, evil);
    }

    FLASH_Lock();//lock the flash for writing
    return i;
}

uint16_t flash_read_buffer(uint8_t* buffer, uint16_t len) {
    uint16_t i;
    uint16_t evil;

    for (i = 0; i < (len > PAGE_SIZE ? PAGE_SIZE : len); i+=2) {
        evil = *((uint16_t*) (FLASH_START_ADDRESS + i));
        buffer[i] = (uint8_t) (evil >> 8);
        buffer[i + 1] = (uint8_t) evil;
    }

    return i;
}
