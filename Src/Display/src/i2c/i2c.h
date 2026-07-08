#ifndef I2C_H
#define I2C_H

#include <stdint.h>

/**
 Low-level I2C driver for STM32 (register level implementation).
 *
 * This module provides basic I2C functionality for STM32 microcontrollers,
 * including peripheral configuration and byte data transfers.
 *
 * System clock switch  should be set to 16MHz
 *
 *	// EXAMPLE USAGE:
 * 		// setup:
 * 		#define SLAVE_ADDRESSS_7BIT 0b0111100U
 * 		I2C_configure();
 * 		...
 * 		// data transfer:
 * 		#define DATA_SIZE_BYTES 2
 * 		uint8_t data[DATA_SIZE_BYTES] = {0x00, 0xAF};
 * 		I2C_init_transfer(DATA_SIZE_BYTES, SLAVE_ADDRESSS_7BIT);
 * 		for (int i = 0; i < DATA_SIZE_BYTES; i++)
 * 			I2C_send_byte(data[i]);
 * 		I2C_wait_for_transfer_end();
 */

void I2C_configure(void);

void I2C_init_transfer(uint8_t bytes_num, uint8_t slave_7bit_address);

// Should be called after any new I2c transfer.
void I2C_wait_for_transfer_end(void);

void I2C_send_byte(uint8_t byte);

#endif /*I2C_H */
