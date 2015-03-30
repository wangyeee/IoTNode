#include "stm32f0xx.h"

static __IO uint32_t sysTickCounter;

void init_delay(void) {
    /* SystemFrequency/1000 1ms */
    while (SysTick_Config(SystemCoreClock / 1000) != 0);
}

/**
  * @brief This function handles SysTick Handler.
  * @param None
  * @retval None
  */
void SysTick_Handler(void) {
    if (sysTickCounter) {
        sysTickCounter--;
    }
}

void delay_ms(uint32_t ms) {
    sysTickCounter = ms;
    while (sysTickCounter);
}
