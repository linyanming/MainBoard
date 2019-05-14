

#include "adc.h"


#define ADC_DR_Address (ADC1_BASE + 0x40)

u16 ADC_Value;   //ADC转化后的值

/************************************************
ADC初始化函数

参数：
	无

返回值：
	无
*************************************************/

void Speed_ADC_Init(void)
{
	GPIO_InitTypeDef GPIO_Str;
	ADC_InitTypeDef ADC_Str;
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

	GPIO_Str.GPIO_Mode = GPIO_Mode_AN;
	GPIO_Str.GPIO_Pin = GPIO_Pin_1;
	GPIO_Str.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Str.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_Str);

	ADC_DeInit(ADC1);
	ADC_Str.ADC_ContinuousConvMode = ENABLE;
	ADC_Str.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_Str.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_Str.ADC_Resolution = ADC_Resolution_12b;
	ADC_Str.ADC_ScanDirection = ADC_ScanDirection_Upward;
	ADC_Init(ADC1, &ADC_Str);

	ADC_ChannelConfig(ADC1, ADC_Channel_9, ADC_SampleTime_239_5Cycles);

	ADC_GetCalibrationFactor(ADC1);

	DMA_ADC_Config();
	ADC_DMARequestModeConfig(ADC1, ADC_DMAMode_Circular);
	ADC_DMACmd(ADC1, ENABLE);

	ADC_Cmd(ADC1, ENABLE);

	while(!ADC_GetFlagStatus(ADC1, ADC_FLAG_ADRDY));

	ADC_StartOfConversion(ADC1);

	DEBUGMSG("ADC Init");
}

/************************************************
针对ADC的DMA初始化函数
参数：
	无

返回值：
	无
*************************************************/

static void DMA_ADC_Config(void)
{
	DMA_InitTypeDef DMA_Str;
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	DMA_DeInit(DMA1_Channel1);

	DMA_Str.DMA_BufferSize = 1;               //数据数目    : 1
	DMA_Str.DMA_DIR = DMA_DIR_PeripheralSRC;  //传输方向: 外设到DMA
	DMA_Str.DMA_M2M = DMA_M2M_Disable;        
	DMA_Str.DMA_MemoryBaseAddr = (u32)&ADC_Value;  //内存储存地址
	DMA_Str.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;   //储存大小：半字 即：两个字节
	DMA_Str.DMA_MemoryInc = DMA_MemoryInc_Disable;  //储存地址不递增，目前只存放一个数据
	DMA_Str.DMA_Mode = DMA_Mode_Circular;   //循环模式
	DMA_Str.DMA_PeripheralBaseAddr = (u32)ADC_DR_Address;  //外设地址
	DMA_Str.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  //外设数据大小：半字 即：两个字节
	DMA_Str.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_Str.DMA_Priority = DMA_Priority_High;  //优先级
	DMA_Init(DMA1_Channel1, &DMA_Str);

	DMA_Cmd(DMA1_Channel1, ENABLE);
}


/************************************************
ADC采样数值获取函数
参数：
	无

返回值：
	ADC转换值
*************************************************/
u16 Get_ADC_Value(void)
{
	while(DMA_GetFlagStatus(DMA1_FLAG_TC1) == RESET);
	DMA_ClearFlag(DMA1_FLAG_TC1);

//	DEBUGMSG("GET adc value = %d", ADC_Value);

	return ADC_Value;
}

