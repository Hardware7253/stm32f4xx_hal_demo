#include <stdint.h>
#include <stdbool.h>

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
        htim2.Init.Prescaler = 8399; // 84 MHz / prescaler = x
        htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
        htim2.Init.Period = 9000; // x / period = timer reset period
        htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        htim2.Init.RepetitionCounter = 0;
        htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

        error_handler_msg(HAL_TIM_Base_Init(&htim2), "Failed to init TIM");

        HAL_NVIC_SetPriority(TIM2_IRQn, 1, 0);
        HAL_NVIC_EnableIRQ(TIM2_IRQn);

        error_handler_msg(HAL_TIM_Base_Start_IT(&htim2), "Failed to start TIM");
    }

    // ADC config
    {
        GPIO_InitTypeDef pin_cfg;
        pin_cfg.Pin = GPIO_PIN_0;
        pin_cfg.Mode = GPIO_MODE_ANALOG;
        pin_cfg.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(GPIOA, &pin_cfg);

        // Clocked by APB2 peripheral clock (84MHz)
        __HAL_RCC_ADC1_CLK_ENABLE();

        hadc1.Instance = ADC1;
        hadc1.Init.Resolution = ADC_RESOLUTION_12B;
        hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2; // 42 MHz
        hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
        hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
        hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T2_CC2;
        hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_RISING;

        ADC_ChannelConfTypeDef channel_cfg = {
            .Channel = ADC_CHANNEL_2,
            .Rank = 1,
            .SamplingTime = ADC_SAMPLETIME_15CYCLES, // 2.8 Msps at 42 MHz (one sample every ~643 ns)
        };

        HAL_ADC_Init(&hadc1);
        HAL_ADC_ConfigChannel(&hadc1, &channel_cfg);
        HAL_ADC_Start(&hadc1);
        
        // HAL_ADC_Start_DMA(&hadc1, &data, 4);
        // uint32_t data;
        // HAL_ADC_PollForConversion(&hadc1, 5);
        // data = HAL_ADC_GetValue(&hadc1);
    }

    // 2 Hz software timer
    software_timer_t stimer = construct_stimer(1000 / HAL_GetTickFreq(), 2, HAL_GetTick(), PERIODIC_ST, calc_ticks_ff);

    // This would be a 2 second period software timer
    // software_timer_t stimer = construct_stimer(1000 / HAL_GetTickFreq, 2000, HAL_GetTick(), PERIODIC_ST, calc_ticks_ft);

    // bool led_state = false;
    uint32_t data;
    while (true) {



        // Led will flash at 1/2 software timer frequency (1 Hz)
        if (is_stimer_finished(&stimer, HAL_GetTick())) {
            HAL_ADC_Start_DMA(&hadc1, &data, 4);
            volatile uint32_t test = data;
            // HAL_ADC_Start(&hadc1);
            // HAL_ADC_PollForConversion(&hadc1, 5);
            // data = HAL_ADC_GetValue(&hadc1);
            // led_state = !led_state;
            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_1);
            (void) test;
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
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_1);
    }
}
