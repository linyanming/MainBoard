

#ifndef __USART_H__
#define __USART_H__

#include "stm32f0xx_rcc.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_usart.h"
#include "config.h"


#ifdef USART_DEBUG
int fputc(int ch,FILE* p);
#endif

void Usart1_Init(u32 baud);


#endif
