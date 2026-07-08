#ifndef CLOCK_H
#define CLOCK_H

#include <stdint.h>
#include <stdbool.h>

#include "stm32u031xx.h"

// increases by 1 every millisecond
extern volatile uint32_t G_Tick;

// configures system clock to HSI16 - 16MHz
void CLOCK_configure(void);

// configures
void CLOCK_configure_1_wire(void);

void SysTick_Handler(void);

void Delay(uint32_t delay_ms);

static inline void Delay_us(uint16_t delay_us) {
	uint16_t start = TIM16->CNT;

	// because both values are 16bit, and max CNT is 0xFFFF wrap around works
	// for example (uint16_t)((uint16_t)14 - (uint16_t)65530) = 20
	while ((uint16_t) (TIM16->CNT - start) < delay_us) {
		;
	}

}

#endif /* CLOCK_H */
