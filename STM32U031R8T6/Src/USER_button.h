#ifndef USER_BUTTON_H
#define USER_BUTTON_H

#include <stdint.h>
#include <stdbool.h>

typedef enum EButtonState {
	ButtonState_IDLE = 0, // not pressed
	ButtonState_PRESSED, // just pressed
	ButtonState_HELD, // pressed
	ButtonState_RELEASED // just released after pressing
} EButtonState;

extern EButtonState G_USER_button_state;
extern uint32_t G_USER_button_time_pressed_ms;

void USER_button_configure(void);

bool USER_button_pressed(void);

void USER_button_update();

#endif /* USER_BUTTON_H */
