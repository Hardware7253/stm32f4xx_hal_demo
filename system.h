#pragma once

#include "stm32f4xx_hal_def.h"
void init_clocks(void);
void error_handler(HAL_StatusTypeDef status);
void error_handler_msg(HAL_StatusTypeDef status, volatile char* msg);