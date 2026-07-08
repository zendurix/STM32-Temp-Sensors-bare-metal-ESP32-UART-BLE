#include "i2c.h"

#include "stm32u031xx.h"
#include "board_config.h"

static void I2C_configure_pins(void) {
	// GPIO B port clock enable
	RCC->IOPENR |= RCC_IOPENR_GPIOBEN;

	// I2C clock enable
	RCC->APBENR1 |= RCC_APBENR1_I2C1EN;

	// GPIO port mode - set to 0b10 - Alternate function
	GPIOB->MODER &= ~(GPIO_MODER_MODE8);
	GPIOB->MODER |= GPIO_MODER_MODE8_1;

	GPIOB->MODER &= ~(GPIO_MODER_MODE9);
	GPIOB->MODER |= GPIO_MODER_MODE9_1;

	// GPIO alternate function high register (ports 8 to 15 -> [1])
	// set to 0b0100 (AF4 for I2C on PB 8 and 9)
	GPIOB->AFR[1] &= ~(GPIO_AFRH_AFSEL8);
	GPIOB->AFR[1] |= GPIO_AFRH_AFSEL8_2;

	GPIOB->AFR[1] &= ~(GPIO_AFRH_AFSEL9);
	GPIOB->AFR[1] |= GPIO_AFRH_AFSEL9_2;

	// port output type - set to 1 - open-drain
	GPIOB->OTYPER |= GPIO_OTYPER_OT8;

	GPIOB->OTYPER |= GPIO_OTYPER_OT9;

	// set output speed to 0 - low speed (for 100kbs I2C)
	GPIOB->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED8);

	GPIOB->OSPEEDR &= ~(GPIO_OSPEEDR_OSPEED9);

	// set port pull up/down to 0b01 -  pull up
	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD8);
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD8_0;

	GPIOB->PUPDR &= ~(GPIO_PUPDR_PUPD9);
	GPIOB->PUPDR |= GPIO_PUPDR_PUPD9_0;
}

void I2C_configure(void) {
	I2C_configure_pins();

	// 1. clear enable bit to allow configuration
	I2C1->CR1 &= ~(I2C_CR1_PE);

	// 2. analog noise filter - set to 0 - enable
	I2C1->CR1 &= ~(I2C_CR1_ANFOFF);

	// 2. digital noise filter - set to 0b0000 - disable
	I2C1->CR1 &= ~(I2C_CR1_DNF);

	// 3. Timing values taken from generating project in MX (clock HSI 16 MHz, I2C 100KHz) :   hi2c1.Init.Timing = 0x00303D5B;
	// I2C1->TIMINGR = 0x00303D5B;
	I2C1->TIMINGR =
	// PRESC 0b0000 = 0 timing prescaler
			(0U << I2C_TIMINGR_PRESC_Pos) |
			// SCLDEL 0b0011 = 3 data setup time
					(3U << I2C_TIMINGR_SCLDEL_Pos) |
					// SDADEL 0b0000 = 0 data hold time
					(0U << I2C_TIMINGR_SDADEL_Pos) |
					// SCLH 0b0011 1102 = 61 SCL high period
					(61U << I2C_TIMINGR_SCLH_Pos) |
					// SCLL 0b0101 1011 = 91 SCL low period
					(91U << I2C_TIMINGR_SCLL_Pos);

	// 4. peripherals enable - set to 1 - enable
	I2C1->CR1 |= I2C_CR1_PE;
}

void I2C_init_transfer(uint8_t bytes_num, uint8_t slave_7bit_address) {
	// address must be 7bit only
	if (slave_7bit_address > 0x7F) {
		return;
	}

	// wait if busy
	while (I2C1->ISR & I2C_ISR_BUSY) {
		;
	}

	// 10 bit addressing mode - set to 0 - use 7bit addressing mode
	I2C1->CR2 &= ~(I2C_CR2_ADD10);

	// target address (control mode) - shift address bit by 1 left - according to I2C doc, address is in bits [7:1]
	I2C1->CR2 &= ~(I2C_CR2_SADD);
	I2C1->CR2 |= (slave_7bit_address << 1);

	// transfer direction (control mode) - set to 0 - write transfer
	I2C1->CR2 &= ~(I2C_CR2_RD_WRN);

	// number of bytes to transfer
	I2C1->CR2 &= ~(I2C_CR2_NBYTES);
	I2C1->CR2 |= (bytes_num << I2C_CR2_NBYTES_Pos);

	// auto end enable - set to 1. Automatically sends STOP when NBYTES are sent
	I2C1->CR2 |= I2C_CR2_AUTOEND;

	// disable reload
	I2C1->CR2 &= ~(I2C_CR2_RELOAD);

	// set start of communication 1
	I2C1->CR2 |= I2C_CR2_START;
}

void I2C_wait_for_transfer_end(void) {

	while (!(I2C1->ISR & I2C_ISR_STOPF)) {
		;
	}
	I2C1->ICR = I2C_ICR_STOPCF;
}

void I2C_send_byte(uint8_t byte) {
	// Transmit interrupt status - set to 1 after previous data sending ended
	while ((I2C1->ISR & I2C_ISR_TXIS) == 0) {
		;
	}
	I2C1->TXDR = byte;
}
