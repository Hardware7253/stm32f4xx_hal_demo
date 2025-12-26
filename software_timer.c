#include "software_timer.h"

// Calculate the ticks elapsed to create a new frequency (timer_f)
// Expects tick_f to be higher than timer_f
// Expects tick_f in Hz and timer_f in Hz 
uint32_t calc_ticks_ff(uint16_t tick_f, uint16_t timer_f) {
    return tick_f / timer_f;
}

// Calculate the pacer ticks elapsed to create a new period (timer_t)
// Expects tick_f in Hz and timer_t in ms
uint32_t calc_ticks_ft(uint16_t tick_f, uint16_t timer_t) {
    return timer_t * tick_f / 1000; 
}

// Constructs software timer, uses one of the functions above to calculate period ticks
software_timer_t construct_stimer(
    uint16_t tick_f, 
    uint16_t timer_u, 
    uint32_t init_ticks,
    software_timer_mode_t mode, 
    calc_ticks calc_ticks_fn
) {
    uint32_t period_ticks = (*calc_ticks_fn)(tick_f, timer_u);

    software_timer_t new_timer = {
        .mode = mode,
        .period_ticks = period_ticks,
        .rollover_ticks = init_ticks + period_ticks,
    };

    return new_timer;
}

// Resets a timer to run again at the rate it was initialised with
void reset_timer(software_timer_t* software_timer, uint32_t current_ticks) {
    software_timer->rollover_ticks = current_ticks + software_timer->period_ticks;

    if (software_timer->mode == DISABLED_ST) {
        software_timer-> mode = ONESHOT_ST;
    }
}

// Return true if the timer has finished
// Also resets periodic timers so they will continue to run
bool is_stimer_finished(software_timer_t* software_timer, uint32_t current_ticks) {
    if (software_timer->mode == DISABLED_ST) {
        return false;
    }

    if (current_ticks >= software_timer->rollover_ticks) {

        if (software_timer->mode == PERIODIC_ST) {
            reset_timer(software_timer, current_ticks);
        } else {
            software_timer->mode = DISABLED_ST;
        }

        return true;
    }

    return false;
}

// Returns the progress of the timer (0-100)
uint8_t get_stimer_progress(software_timer_t* software_timer, uint32_t current_ticks) {
    uint8_t progress = ((uint64_t)current_ticks * 100) / software_timer->period_ticks;
    if (progress > 100) {
        progress = 100; // Clamp range
    }

    return progress;
}