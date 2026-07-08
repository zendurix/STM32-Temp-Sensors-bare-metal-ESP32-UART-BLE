#include "LD_4.h"

#include "stm32u031xx.h"

#include "clock.h"

bool G_LD4_state = false;
uint32_t G_LD4_blinking_toggle_time_stamp = 0;
uint32_t G_LD4_blinking_toggle_time = 1000;

// LD4 - pin PA5

void LD4_configure(void) {
	// GPIO A port clock enable
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;

	// GPIO port mode - set to 01 - Output
	GPIOA->MODER &= ~(GPIO_MODER_MODE5);
	GPIOA->MODER |= GPIO_MODER_MODE5_0;

	// port output type - set to 0 - push-pull
	GPIOA->OTYPER &= ~(GPIO_OTYPER_OT5);

	// set output speed to low - 00
	GPIOA->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED5);

	// set port pull up/down to no pull up, pull down 00
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD5);

	// turn off
	LD4_set(false);
}

void LD4_set(bool turn_on) {
	if (turn_on) {
		// set - 1
		GPIOA->BSRR = GPIO_BSRR_BS5;
	} else {
		// reset = 0
		GPIOA->BSRR = GPIO_BSRR_BR5;
	}
	G_LD4_state = turn_on;
}

void LD4_toggle(void) {
	GPIOA->ODR ^= GPIO_ODR_OD5;
	G_LD4_state = !G_LD4_state;

	G_LD4_blinking_toggle_time_stamp = G_Tick;
}
