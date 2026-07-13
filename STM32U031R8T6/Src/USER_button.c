#include "USER_button.h"
#include <stdbool.h>

#include "stm32u031xx.h"

#include "clock.h"

EButtonState G_USER_button_state = ButtonState_IDLE;
uint32_t G_USER_button_time_pressed_ms = 0;

uint32_t USER_button_pressed_time_stamp = 0;

// USER button - pin PC13

void USER_button_configure(void) {
	// GPIO C port clock enable
	RCC->IOPENR |= RCC_IOPENR_GPIOCEN;

	// MODER 00 - input mode
	GPIOC->MODER &= ~(GPIO_MODER_MODE13);

	/// PUPDR 01 - pull up (1 not pressed, 0 pressed)
	GPIOC->PUPDR &= ~(GPIO_PUPDR_PUPD13);
	GPIOC->PUPDR |= GPIO_PUPDR_PUPD13_0;
}

bool USER_button_pressed() {
	return !(GPIOC->IDR & GPIO_IDR_ID13);
}

void USER_button_update() {
	bool is_pressed = USER_button_pressed();

	switch (G_USER_button_state) {
	case ButtonState_IDLE:
		G_USER_button_time_pressed_ms = 0;
		G_USER_button_state =
				(is_pressed) ? ButtonState_PRESSED : ButtonState_IDLE;
		break;
	case ButtonState_PRESSED:
		USER_button_pressed_time_stamp = G_Tick;
		G_USER_button_state =
				(is_pressed) ? ButtonState_HELD : ButtonState_RELEASED;
		break;
	case ButtonState_HELD:
		G_USER_button_time_pressed_ms = G_Tick - USER_button_pressed_time_stamp;
		G_USER_button_state =
				(is_pressed) ? ButtonState_HELD : ButtonState_RELEASED;
		break;
	case ButtonState_RELEASED:
		G_USER_button_state =
				(is_pressed) ? ButtonState_PRESSED : ButtonState_IDLE;
		break;
	}
}
