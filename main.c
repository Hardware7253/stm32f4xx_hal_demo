#include <stdint.h> #include <stdbool.h>

#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_conf.h"

#include "software_timer.h"
#include "system.h"
#include "bit.h"

TIM_HandleTypeDef htim2;
ADC_HandleTypeDef hadc1;
static bool led_state = false;

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

    // Timer config
    {
        __HAL_RCC_TIM2_CLK_ENABLE();

        // TIM2 is clocked by APB1 timer clock (84MHz)
        htim2.Instance = TIM2;
        htim2.Init.Prescaler = 8399; // 84 MHz / (prescaler + 1) = 10 kHz
        htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
        htim2.Init.Period = 999; // 10 kHz / (period + 1) = 10 Hz
        htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

        error_handler_msg(HAL_TIM_Base_Init(&htim2), "Failed to init TIM");

        HAL_NVIC_SetPriority(TIM2_IRQn, 1, 0);
        HAL_NVIC_EnableIRQ(TIM2_IRQn);
        error_handler_msg(HAL_TIM_Base_Start_IT(&htim2), "Failed to start TIM");
    }

    // 2 Hz software timer
    software_timer_t stimer = construct_stimer(1000 / HAL_GetTickFreq(), 2, HAL_GetTick(), PERIODIC_ST, calc_ticks_ff);

    // This would be a 2 second period software timer
    // software_timer_t stimer = construct_stimer(1000 / HAL_GetTickFreq, 2000, HAL_GetTick(), PERIODIC_ST, calc_ticks_ft);

    while (true) {

        // Led will flash at 1/2 software timer frequency (1 Hz)
        if (is_stimer_finished(&stimer, HAL_GetTick())) {
            // HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_1);
        }
    }

    return 0;
}

extern void TIM2_IRQHandler() {
    HAL_TIM_IRQHandler(&htim2);
}

// Callback started by HAL ISR
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM2) {

        // Output wave frequency is 1/2 timer frequency due to toggle
        // But the interrupt triggers at each edge (2x output wave frequency)
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_1);
    }
}
