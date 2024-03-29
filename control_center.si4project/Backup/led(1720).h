

#ifndef __LED_H__
#define __LED_H__

#include "stm32f10x_gpio.h"
#include "bit.h"
#include "config.h"

#define LED1   PBout(12)
#define LED2   PBout(13)
#define LED3   PBout(14)
#define LED4   PBout(15)

#define RGB1   PAout(8)
#define RGB2   PAout(9)
#define RGB3   PAout(10)

void Led_Init(void);
void Led_Reset(void);

#endif
