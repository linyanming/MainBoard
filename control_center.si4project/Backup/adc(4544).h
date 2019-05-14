
#ifndef __ADC_H__
#define __ADC_H__

#include "stm32f0xx_adc.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_rcc.h"
#include "config.h"


void Speed_ADC_Init(void);
static void DMA_ADC_Config(void);
u16 Get_ADC_Value(void);


#endif

