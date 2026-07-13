#include "1_wire.h"

#include "stm32u031xx.h"
#include "../../clock.h"

#define MIN_TIME_SLOT_US Delay_us(70)

void wire1_initialize(void) {
	// PA0

	// enable clock on GPIO A
	RCC->IOPENR |= RCC_IOPENR_GPIOAEN;

	// Set MODER to 0b01 - output mode
	GPIOA->MODER &= ~(GPIO_MODER_MODE0);
	GPIOA->MODER |= (0b01 << GPIO_MODER_MODE0_Pos);

	// PUPDR 0b00 no pull up no pull down (for 1 wire use external PULL UP 4.7 k Ohm)
	GPIOA->PUPDR &= ~(GPIO_PUPDR_PUPD0);

	// OTYPER 0b1 - output open drain
	GPIOA->OTYPER |= GPIO_OTYPER_OT0;
}

void wire1_pull_LOW(void) {
	// output mode
	GPIOA->MODER &= ~(GPIO_MODER_MODE0);
	GPIOA->MODER |= (0b01 << GPIO_MODER_MODE0_Pos);

	// reset
	GPIOA->BSRR = GPIO_BSRR_BR0;
}

void wire1_release_bus(void) {
	// 0b00 input mode
	GPIOA->MODER &= ~(GPIO_MODER_MODE0);
	// pull up resistor raises HIGH
}

void wire1_read_time_slot_init() {
	wire1_pull_LOW();
	Delay_us(10);
	wire1_release_bus();
	Delay_us(10);
}

uint8_t wire1_read_bit(void) {
	uint8_t bit = GPIOA->IDR & GPIO_IDR_ID0;
	return bit;
}

void wire1_initialization_sequence(void) {
	// reset pulse - min 480 us
	wire1_pull_LOW();
	Delay_us(490);
	wire1_release_bus();

	// DS18B20 waits15-60us, and transmits presence - LOW for 60-240us
	Delay_us(65);

	// should handle errors here
	while (!wire1_read_bit()) {
		;
	}

	Delay_us(400);
}

void wire1_send_byte(uint8_t byte) {
	for (uint8_t bit_pos = 0; bit_pos < 8; bit_pos++) {
		wire1_pull_LOW();
		if ((byte >> bit_pos) & 1) {
			wire1_release_bus();
			Delay_us(70);
		} else {
			Delay_us(65);
			wire1_release_bus();
		}
		Delay_us(5);
	}
}

uint8_t wire1_read_byte() {
	uint8_t byte = 0U;

	for (uint8_t bit_pos = 0; bit_pos < 8; bit_pos++) {
		wire1_read_time_slot_init();
		uint8_t bit = wire1_read_bit();
		if (bit) {
			byte |= (1 << bit_pos);
		}
		Delay_us(60);
	}

	return byte;
}
