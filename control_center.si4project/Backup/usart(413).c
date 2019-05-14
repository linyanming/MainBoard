
#include "usart.h"

//串口输出重定向函数  用于printf打印输出到串口
#ifdef USART_DEBUG
int fputc(int ch,FILE* p)
{
	USART_SendData(USART1, (u8)ch);
	while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) != SET);
	return ch;
}
#endif

/*********************************
功能：
	串口1初始化

参数：
	baud：波特率
返回值：无

**********************************/

void Usart1_Init(u32 baud)
{
	GPIO_InitTypeDef GPIO_Str;
	USART_InitTypeDef USART_Str;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	GPIO_Str.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Str.GPIO_OType = GPIO_OType_PP;
	GPIO_Str.GPIO_Pin = GPIO_Pin_2;
	GPIO_Str.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Str.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_Str);

	GPIO_Str.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Str.GPIO_Pin = GPIO_Pin_3;
	GPIO_Str.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Str.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_Str);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_0);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_0);

	USART_Str.USART_BaudRate = baud;
	USART_Str.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_Str.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Str.USART_Parity = USART_Parity_No;
	USART_Str.USART_StopBits = USART_StopBits_1;
	USART_Str.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_Str);

	USART_Cmd(USART1, ENABLE);
}


