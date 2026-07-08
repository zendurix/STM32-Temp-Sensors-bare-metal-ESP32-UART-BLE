#include <stdint.h>
#include <stdbool.h>
// snprintf
#include <stdio.h>

#include "stm32u031xx.h"

#include "LD_4.h"
#include "USER_button.h"
#include "clock.h"
#include "Display/display.h"
#include "TempAnalogSensor/temp_analog_sensor.h"
#include "TempDigitalSensor/temp_digital_sensor.h"

/*
 Board used - Nucleo STM32U031R8T6.
 Connections:
 OLED VCC -> Board 3V3
 OLED GND -> Board GND
 OLED SDA -> Board D14 (PB9 on processor)
 OLED SCL -> Board D15 (PB8 on processor)

 */

#include "TempDigitalSensor/1_wire/1_wire.h"

#define TOGGLE_PROGRAM_MODE_PRESS_TIME 1000

int main(void) {
	CLOCK_configure();
	CLOCK_configure_1_wire();

	// 16Mhz base, 1ms interrupts
	SysTick_Config(16000000 / 1000);

	LD4_configure();
	USER_button_configure();

	TempAnalogSens_initialize();
	TempDigitalSens_initialize();

	float temp_analog = 10.213;
	float temp_digital = 66.213;
	float temp_cpu = 101.11;
	uint8_t byte_digi;
	bool temp_changed = true;

	Display *display = Display_create(100, 0, 0, G_Tick);
	if (!display) {
		return 1;
	}

	Display_set_mode(display, DisplayMode_TEXT);

	G_USER_button_state = ButtonState_PRESSED;

	while (true) {
		USER_button_update();
		if (temp_changed) {
			char buff[20];

			Display_clear_text(display);
			Display_add_string_to_text(display, "-=TEMPERATURE=-", 0, 0);

			Display_add_string_to_text(display, "A: ", 0, 1);
			int t_x100 = temp_analog * 100;
			snprintf(buff, 32, "%d.%02d%c", t_x100 / 100, t_x100 % 100, 'C');
			Display_add_string_to_text(display, buff, 3, 1);

			Display_add_string_to_text(display, "D: ", 0, 2);
			t_x100 = temp_digital * 100;
			snprintf(buff, 32, "%d.%02d%c", t_x100 / 100, t_x100 % 100, 'C');
			Display_add_string_to_text(display, buff, 3, 2);

			Display_add_string_to_text(display, "CPU: ", 0, 3);
			t_x100 = temp_cpu * 100;
			snprintf(buff, 32, "%d.%02d%c", t_x100 / 100, t_x100 % 100, 'C');
			Display_add_string_to_text(display, buff, 5, 3);

			temp_changed = false;
		}

		Delay(1000);

		temp_digital = TempDigitalSens_get_temperature(&byte_digi);
		temp_analog = TempAnalogSens_get_temperature();
		temp_cpu = GetInternalTemp();
		temp_changed = true;

		if (G_USER_button_state == ButtonState_RELEASED) {
			if (G_USER_button_time_pressed_ms > TOGGLE_PROGRAM_MODE_PRESS_TIME) {
				Display_toggle_on_off(display);
			} else {
				temp_digital = TempDigitalSens_get_temperature(&byte_digi);
				temp_analog = TempAnalogSens_get_temperature(); // TempAnalogSens_get_temperature();
				temp_cpu = GetInternalTemp();
				temp_changed = true;
			}
		}
		Display_update(display, G_Tick);
	}
}
