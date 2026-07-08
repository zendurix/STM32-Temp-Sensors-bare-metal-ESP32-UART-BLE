#include "clock.h"

volatile uint32_t G_Tick = 0;

void CLOCK_configure(void) {
	// APB prescaler - set to 000 - 1
	RCC->CFGR &= ~(RCC_CFGR_PPRE);

	// AHB prescaler - set to 000 - 1
	RCC->CFGR &= ~(RCC_CFGR_HPRE);

	// enable HSI16 clock - 1
	RCC->CR |= RCC_CR_HSION;

	// wait until HSI16 clock is ready
	while ((RCC->CR & RCC_CR_HSIRDY) == 0)
		;

	// System clock switch - set to 001 - HSI16 - 16MHz
	RCC->CFGR &= ~(RCC_CFGR_SW);
	RCC->CFGR |= RCC_CFGR_SW_0;

	// wait for System clock switch status to acknowledge HSI16 (0x001 - HSI16)
	while (((RCC->CFGR & RCC_CFGR_SWS) >> RCC_CFGR_SWS_Pos) != 0x001)
		;
}

void CLOCK_configure_1_wire(void) {
	// enable TIM16
	RCC->APBENR2 |= RCC_APBENR2_TIM16EN;
	// force read register to be sure clock enable was propagated
	(void)RCC->APBENR2;

	// disable timer before config
	TIM16->CR1 &= ~(TIM_CR1_CEN);

	// set prescaler: 16MHz / (15+1) = 1Mhz -> 1us counter
	TIM16->PSC = 15;

	// Auto reload value (max value - ~65ms)
	TIM16->ARR = 0xFFFF;

	// update prescaler
	TIM16->EGR = TIM_EGR_UG;

	// reset counter
	TIM16->CNT = 0;

	// start timer
	TIM16->CR1 |= TIM_CR1_CEN;
}

void SysTick_Handler(void) {
	G_Tick++;
}

void Delay(uint32_t delay_ms) {
	volatile uint32_t start_time = G_Tick;

	while (G_Tick < (start_time + delay_ms)) {
		// do nothing
		// WFI - wait for interrupt
		__WFI();
	}
}

