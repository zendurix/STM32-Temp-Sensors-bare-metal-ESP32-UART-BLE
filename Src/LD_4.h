#ifndef LD_4_H
#define LD_4_H

#include <stdint.h>
#include <stdbool.h>

extern bool G_LD4_state;
extern uint32_t G_LD4_blinking_toggle_time_stamp;
extern uint32_t G_LD4_blinking_toggle_time;

void LD4_configure(void);

void LD4_set(bool turn_on);

void LD4_toggle(void);

#endif /* LD_4_H */
