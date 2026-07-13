#include "UART.h"

#include "stm32u031xx.h"
/*
ESP32:
	PA5 (D13 nucleo) -> USART3 TX (RX  ESP32)
	PA7 (D11 nucleo) -> USART3 RX (TX  ESP32)

PC (ST-LINK):
	PA 2 -> TX
	PA3 -> RX

	BR 115200
	oversampling 16
*/


void UART_initialize(void) {
	// GPIO A port clock enable
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;

	// set both pins to alternate mode 0b10
	GPIOA->MODER &= ~(GPIO_MODER_MODE5);
	GPIOA->MODER |= GPIO_MODER_MODE5_1;

	GPIOA->MODER &= ~(GPIO_MODER_MODE7);
	GPIOA->MODER |= GPIO_MODER_MODE7_1;

	// OTYPER, OSPEEDR< PUPDR ok from default reset values

	// AF USART = 7 0b0111
	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL5);
	GPIOA->AFR[0] |= (0b0111 << GPIO_AFRL_AFSEL5_Pos);

	GPIOA->AFR[0] &= ~(GPIO_AFRL_AFSEL7);
	GPIOA->AFR[0] |= (0b0111 << GPIO_AFRL_AFSEL7_Pos);

	//

	// enable USART3 clock
	RCC->APBENR1 |= RCC_APBENR1_USART3EN;

	// disable UART before config
	USART3->CR1 &= ~(USART_CR1_UE);

	// M1 M0 Word Lenght = 0b00 - 8 data bits
	USART3->CR1 &= ~(USART_CR1_M1);
	USART3->CR1 &= ~(USART_CR1_M0);

	// USART_CR1_OVER8 0b0 - oversampling by 16
	USART3->CR1 &= ~(USART_CR1_OVER8);

	// 0b00 - 1 stop bit
	USART3->CR2 &= ~(USART_CR2_STOP);

	// Disable clock (used for USART only)
	USART3->CR2 &= ~(USART_CR2_CLKEN);

	// 16Mhz / BR 115200 = 138.888888...
	USART3->BRR = 139;

	// ENABLE UART   TE - trnsmitter enable, RE - receiver enable, UE - uart enable
	USART3->CR1 |= USART_CR1_TE;
	USART3->CR1 |= USART_CR1_RE;
	USART3->CR1 |= USART_CR1_UE;
}

void UART_send_string(char *msg) {
	if (!msg) {
		return;
	}

	for (char *character = msg; *character != '\0'; character++) {
		UART_send_byte((uint8_t) (*character));
	}
}

void UART_send_byte(uint8_t byte) {
	// waint for tranfser end
	while ((USART3->ISR & USART_ISR_TXE_TXFNF) == 0) {
		;
	}
	USART3->TDR = byte;

	while (!(USART3->ISR & USART_ISR_TC)) {
		;
	}
}

uint8_t UART_receive_byte() {
	if (USART3->ISR & USART_ISR_ORE) {
		USART3->ICR |= USART_ICR_ORECF;
	}

	if (USART3->ISR & USART_ISR_RXNE_RXFNE) {
		return (uint8_t) USART3->RDR;
	} else {
		return '\0';
	}
}

