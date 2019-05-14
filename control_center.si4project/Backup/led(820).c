

#include "led.h"

/********************************
LED初始化函数
**************************************/

void Led_Init(void)
{
	GPIO_InitTypeDef GPIO_Str;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	GPIO_Str.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_Str.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_Str.GPIO_OType = GPIO_OType_PP;
	GPIO_Str.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Str.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_Str);

	DEBUGMSG("LED init");
}


