#ifndef __DEBUG_H
#define __DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include "at32f403a_407.h"

/**************** define print uart ******************/
#define PRINT_UART                       USART1
#define PRINT_UART_CRM_CLK               CRM_USART1_PERIPH_CLOCK
#define PRINT_UART_TX_PIN                GPIO_PINS_9
#define PRINT_UART_TX_GPIO               GPIOA
#define PRINT_UART_TX_GPIO_CRM_CLK       CRM_GPIOA_PERIPH_CLOCK

void uart_print_init(uint32_t baudrate);

#ifdef __cplusplus
}
#endif

#endif

