// main.h
// Josh Brake
// jbrake@hmc.edu
// 10/31/22 

#ifndef MAIN_H
#define MAIN_H

#include "STM32L432KC.h"
#include <stm32l432xx.h>

///////////////////////////////////////////////////////////////////////////////
// Custom defines
///////////////////////////////////////////////////////////////////////////////

#define A_PIN PA8
#define B_PIN PB4
#define COUNTING_TIM TIM2
#define FRAME_TIM TIM6
#define FRAME_MS 500

#define topFactor 947
#define botFactor 740

#endif // MAIN_H