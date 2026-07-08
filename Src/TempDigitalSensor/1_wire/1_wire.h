#ifndef _1_WIRE_WIRE_H_
#define _1_WIRE_WIRE_H_

#include <stdint.h>
#include <stdbool.h>

// All data and commands are transmitted least significant bit first over the 1-Wire bus


void wire1_initialize(void);

void wire1_pull_LOW(void);

void wire1_release_bus(void);

void wire1_read_time_slot_init();

uint8_t wire1_read_bit(void);


void wire1_initialization_sequence(void);

void wire1_send_byte(uint8_t byte);

uint8_t wire1_read_byte();



#endif /* _1_WIRE_WIRE_H_ */
