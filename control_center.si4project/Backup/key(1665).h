

#ifndef __KEY_H__
#define __KEY_H__

#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_exti.h"
#include "config.h"
#include "bit.h"




typedef enum
{
	KEYRELEASE = 0,      //按键释放
	KEYPRESS,        //按键按下
}KeyEvent;

//按键引脚数值读取
#define KEY_PWR    PBin(4)

#define nF  PBin(2)

extern u8 KeyStatus;

void Key_Init(void);


#endif

