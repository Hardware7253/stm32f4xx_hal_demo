#ifndef SOFTWARE_TIMER_H
#define SOFTWARE_TIMER_H

#include <stdint.h>
#include <stdbool.h>

// A oneshot software timer will run once until it reaches rollover_ticks
// Periodic timer will repeat forever
typedef enum {
    ONESHOT_ST,
    PERIODIC_ST,
    DISABLED_ST,
} software_timer_mode_t;

typedef struct {
    software_timer_mode_t mode;
    uint32_t period_ticks;
    uint32_t rollover_ticks;
} software_timer_t;

typedef uint32_t (*calc_ticks)(uint16_t, uint16_t);

uint32_t calc_ticks_ff(uint16_t pacer_f, uint16_t timer_f);
uint32_t calc_ticks_ft(uint16_t pacer_f, uint16_t timer_t);

software_timer_t construct_stimer(
    uint16_t tick_f, 
    uint16_t timer_u, 
    uint32_t init_ticks,
    software_timer_mode_t mode, 
    calc_ticks calc_ticks_fn
);

void reset_timer(software_timer_t* software_timer, uint32_t current_ticks);
bool is_stimer_finished(software_timer_t* software_timer, uint32_t current_ticks);
uint8_t get_stimer_progress(software_timer_t* software_timer, uint32_t current_ticks);


#endif