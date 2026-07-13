#ifndef UART_UART_H_
#define UART_UART_H_

#include <stdint.h>
#include <stdbool.h>


void UART_initialize(void);

void UART_send_string(char *msg);

void UART_send_byte(uint8_t byte);

uint8_t UART_receive_byte();

#endif /* UART_UART_H_ */
