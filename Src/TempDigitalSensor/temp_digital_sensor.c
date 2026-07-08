#include "temp_digital_sensor.h"

#include "stm32u031xx.h"

#include "1_wire/1_wire.h"
#include "../clock.h"

#define ROM_COMMAND_READ_ROM 0x33U
#define ROM_COMMAND_MATCH_ROM 0x55U
#define ROM_COMMAND_SKIP_ROM 0xCCU

#define FUNCTION_COMMAND_CONVERT_T 0x44U
#define FUNCTION_COMMAND_READ_SCRATCHPAD 0xBEU

///  The control line requires a weak pullup resistor since all devices are linked to the bus
// via a 3-state or open-drain port (the DQ pin in the case of the DS18B2

/*
 Step 1. Initialization : reset master -> presence puls slave
 Step 2. ROM Command (followed by any required data exchange)
 Step 3. DS18B20 Function Command (followed by any required data exchange

 READ ROM [33h] (for one 1-wire device. If more are on the same bus, use SEARCH ROM[F0h]
 This command can only be used when there is one slave on the bus. It allows the bus master to read the
 slave’s 64-bit ROM code without using the Search ROM procedure. If this command is used when there
 is more than one slave present on the bus, a data collision will occur when all the slaves attempt to
 respond at the same time

 MATCH ROM [55h]
 The match ROM command followed by a 64-bit ROM code sequence allows the bus master to address a
 specific slave device on a multidrop or single-drop bus. Only the slave that exactly matches the 64-bit
 ROM code sequence will respond to the function command issued by the master; all other slaves on the
 bus will wait for a reset pulse.

 SKIP ROM [CCh]
 The master can use this command to address all devices on the bus simultaneously without sending out
 any ROM code information. For example, the master can make all DS18B20s on the bus perform
 simultaneous temperature conversions by issuing a Skip ROM command followed by a Convert T [44h]
 command.
 Note that the Read Scratchpad [BEh] command can follow the Skip ROM command only if there is a
 single slave device on the bus. In this case, time is saved by allowing the master to read from the slave
 without sending the device’s 64-bit ROM code. A Skip ROM command followed by a Read Scratchpad
 command will cause a data collision on the bus if there is more than one slave since multiple devices will
 attempt to transmit data simultaneously.


 // DS18B20 function commands
 * CONVERT T [44h]
 This command initiates a single temperature conversion. Following the conversion, the resulting thermal
 data is stored in the 2-byte temperature register in the scratchpad memory and the DS18B20 returns to its
 low-power idle state. If the device is being used in parasite power mode, within 10μs (max) after this
 command is issued the master must enable a strong pullup on the 1-Wire bus for the duration of the
 conversion (tCONV) as described in the Powering the DS18B20 section. If the DS18B20 is powered by an
 external supply, the master can issue read time slots after the Convert T command and the DS18B20 will
 respond by transmitting a 0 while the temperature conversion is in progress and a 1 when the conversion
 is done. In parasite power mode this notification technique cannot be used since the bus is pulled high by
 the strong pullup during the conversion.

 READ SCRATCHPAD [BEh]
 This command allows the master to read the contents of the scratchpad. The data transfer starts with the
 least significant bit of byte 0 and continues through the scratchpad until the 9th byte (byte 8 – CRC) is
 read. The master may issue a reset to terminate reading at any time if only part of the scratchpad data is
 needed.



 During the initialization sequence the bus master transmits (TX) the reset pulse by pulling the 1-Wire bus
 low for a minimum of 480μs. The bus master then releases the bus and goes into receive mode (RX).
 When the bus is released, the 5kΩ pullup resistor pulls the 1-Wire bus high. When the DS18B20 detects
 this rising edge, it waits 15μs to 60μs and then transmits a presence pulse by pulling the 1-Wire bus low
 for 60μs to 240μs

 There are two types of write time slots: “Write 1” time slots and “Write 0” time slots. The bus master
 uses a Write 1 time slot to write a logic 1 to the DS18B20 and a Write 0 time slot to write a logic 0 to the
 DS18B20.
 To generate a Write 1 time slot, after pulling the 1-Wire bus low, the bus master must release the 1-Wire
 bus within 15μs. When the bus is released, the 5kΩ pullup resistor will pull the bus high. To generate a
 Write 0 time slot, after pulling the 1-Wire bus low, the bus master must continue to hold the bus low for
 the duration of the time slot (at least 60μs).
 The DS18B20 samples the 1-Wire bus during a window that lasts from 15μs to 60μs after the master
 initiates the write time slot. If the bus is high during the sampling window, a 1 is written to the DS18B20.
 If the line is low, a 0 is written to the DS18B20.


 */

void TempDigitalSens_initialize(void) {
	// PA0
	wire1_initialize();

}

float TempDigitalSens_get_temperature(uint8_t* byte) {
	wire1_initialization_sequence();
	wire1_send_byte(ROM_COMMAND_SKIP_ROM);
	wire1_send_byte(FUNCTION_COMMAND_CONVERT_T);

	wire1_read_time_slot_init();
	// sends 1 when done converting
	while (!wire1_read_bit()) {
		Delay_us(50);
		wire1_read_time_slot_init();
	}
	Delay_us(60);
	//Delay_us(8000);

	uint8_t SCRATCHPAD[9] = { 0 };

	wire1_initialization_sequence();
	wire1_send_byte(ROM_COMMAND_SKIP_ROM);
	wire1_send_byte(FUNCTION_COMMAND_READ_SCRATCHPAD);

	for (int i = 0; i < 9; i++) {
		SCRATCHPAD[i] = wire1_read_byte();
	}
	*byte = SCRATCHPAD[0];

	int32_t temp = SCRATCHPAD[0];
	bool is_minus = 0b11111000 & SCRATCHPAD[1];
	// clear sign bits
	SCRATCHPAD[1] &= 0b00000111;

	temp |= (SCRATCHPAD[1] << 8);
	if (is_minus) {
		temp = -temp;
	}

	return (float)temp * 0.0625;
}

