#ifndef _UTILITY_H_
#define _UTILITY_H_

#ifdef __cplusplus
extern "C" {
#endif

void init_stdio_USART2(void);
void init_delay(void);
void delay_ms(uint32_t ms);

#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line) {
    for (;;);
}
#endif

#ifdef __cplusplus
}
#endif
#endif /* _UTILITY_H_ */
