

#ifndef __LED_H__
#define __LED_H__

#include "stm32f0xx_gpio.h"
#include "bit.h"
#include "config.h"

#define GREEN_LED PAout(4)
#define RED_LED   PAout(5)

void Led_Init(void);

#endif
