

#include "key.h"

u8 KeyStatus = KEYRELEASE;


void Key_Init(void)
{
	GPIO_InitTypeDef GPIO_Str;
	EXTI_InitTypeDef EXTI_Str;
	NVIC_InitTypeDef NVIC_Str;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

	GPIO_Str.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Str.GPIO_Pin = GPIO_Pin_4;
//	GPIO_Str.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_Str);

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource4);

	EXTI_Str.EXTI_Line = EXTI_Line4;
	EXTI_Str.EXTI_LineCmd = ENABLE;
	EXTI_Str.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_Str.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
	EXTI_Init(&EXTI_Str);

	NVIC_Str.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_Str.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Str.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_Str.NVIC_IRQChannelSubPriority = 0;

	NVIC_Init(&NVIC_Str);

//	EXTI_GenerateSWInterrupt();
}


