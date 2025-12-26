#include <stdint.h>
#include <stdbool.h>

#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_conf.h"

#include "software_timer.h"
#include "system.h"
#include "bit.h"

int main(void) {

    HAL_Init();
    init_clocks();
   
    // GPIO config 
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();

        GPIO_InitTypeDef pin_cfg = {
            .Pin =  GPIO_PIN_1,
            .Mode = GPIO_MODE_OUTPUT_PP,
            .Pull = GPIO_NOPULL,
            .Speed = GPIO_SPEED_FREQ_LOW,
            .Alternate = 0,
        };
        HAL_GPIO_Init(GPIOA, &pin_cfg);
    }

    // 2 Hz software timer
    software_timer_t stimer = construct_stimer(1000 / HAL_GetTickFreq(), 2, HAL_GetTick(), PERIODIC_ST, calc_ticks_ff);

    // This would be a 2 second period software timer
    // software_timer_t stimer = construct_stimer(1000 / HAL_GetTickFreq, 2000, HAL_GetTick(), PERIODIC_ST, calc_ticks_ft);

    bool led_state = false;
    while (true) {

        // Led will flash at 1/2 software timer frequency (1 Hz)
        if (is_stimer_finished(&stimer, HAL_GetTick())) {
            led_state = !led_state;
            HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, led_state);
        }
    }

    return 0;
}