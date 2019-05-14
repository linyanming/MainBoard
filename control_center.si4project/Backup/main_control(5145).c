
#include "main_control.h"
#include <stdio.h>
#include "usart.h"
#include "led.h"
#include "systick.h"
#include "pwm.h"
#include "key.h"
#include "adc.h"
#include "iwdg.h"
#include "can.h"

u8 pwr_status;  //停机开机管理变量
u16 pwr_time;  //关机时间计数
u8 ConnStatus; //连接状态
u8 DeviceMode; //控制模式

CommandBuf rxbuf;
ConnectDev condev;

/********************************
相关参数初始化
功能：
	相关参数初始化
	
参数：
	无
	
返回值：
	无
**************************************/
void DeviceStatusInit(void)
{
	pwr_status = BOOT_STOP;  //停机开机管理变量
	pwr_time = 0; //关机时间计数
	ConnStatus = DEVINIT;
	DeviceMode = INCH_MODE;
	memset(&rxbuf,0,sizeof(rxbuf));
	memset(&condev,0,sizeof(condev));
}

/********************************
心跳计数
功能：
	心跳计数
	
参数：
	无
	
返回值：
	无
**************************************/
void HeartBeatCounter(void)
{
	if(ConnStatus == DEVCONN)
	{
		for(u8 i = 0;i < condev.connum;i++)
		{
			condev.dev[i].heart_time++;
			if(condev.dev[i].heart_time > MAX_HEART_TIME)
			{
				ConnStatus = DEVLOSE;
				return;
			}
		}	
	}
}

/********************************
连接设备搜索
功能：
	搜索设备是否已连接
	
参数：
	设备ID
	
返回值：
	成功：储存位置
	失败：0xff
**************************************/
u8 SearchDevice(u8 devid)
{
	for(u8 i = 0;i < condev.connum;i++)
	{
		if(condev.dev[i].dev_id == devid)
		{
			return i;
		}
	}
	return 0xff;
}

/********************************
刷新心跳
功能：
	刷新心跳
	
参数：
	设备ID
	
返回值：
	无
**************************************/
void ReflashHeartBeat(u8 devid)
{
	u8 i;
	if((i = SearchDevice(devid)) != 0xff)
	{
		condev.dev[i].heart_time = 1;
	}
}

/********************************
心跳处理函数
功能：
	心跳处理
	
参数：
	设备ID
	
返回值：
	无
**************************************/
void HeartBeatHandler(u8 devid)
{
	ReflashHeartBeat(devid);
}

/********************************
连接处理函数
功能：
	连接处理
	
参数：
	相关命令结构体
	
返回值：
	无
**************************************/
void ConnectHandler(CommandData* dev)
{
	if(SearchDevice(dev->dev_id) == 0xff)
	{
		condev.dev[condev.connum].dev_id = dev->dev_id;
		condev.dev[condev.connum].heart_time = 1;
		CAN_Send_Msg(NULL, 0, condev.dev[condev.connum].dev_id, CONNECT_ACK);
		ConnStatus = DEVCONN;
	}
}

/********************************
模式改变
功能：
	改变控制模式
	
参数：
	相关命令结构体
	
返回值：
	无
**************************************/
void ModeChangeHandler(CommandData* dev)
{
	if(SearchDevice(dev->dev_id) != 0xff)
	{
		DeviceMode = dev->dev_cmd;
		ReflashHeartBeat(dev->dev_id);
	}
}

/********************************
转向电机控制
功能：
	控制转向
	
参数：
	相关命令结构体
	
返回值：
	无
**************************************/
void OrtateMotorControl(CommandData* dev)
{
	if(SearchDevice(dev->dev_id) != 0xff)
	{
		if(dev->dev_cmd == LEFT_TURN)
		{
			Ortate_Motor_Left();
		}
		else if(dev->dev_cmd == RIGHT_TURN)
		{
			Ortate_Motor_Right();
		}
		else
		{
			Ortate_Motor_Brate();
		}
		ReflashHeartBeat(dev->dev_id);
	}
}

/********************************
主控处理函数
功能：
	控制处理
	
参数：
	无
	
返回值：
	无
**************************************/
void Control_Handler(void)
{
	if(pwr_status == BOOT_RUN)
	{
		if(rxbuf.Rxflag == 1)
		{
			switch(rxbuf.cmd.dev_cmd)
			{
				case HEARTBEAT:
					HeartBeatHandler(rxbuf.cmd.dev_id);
					break;
				case LEFT_TURN:
				case RIGHT_TURN:
				case ORTATE_STOP:
					break;
				case START_MOVE:
				case RETREAT:
				case STOP_MOVE:
					break;
				case SPEED_CNTR:
					break;
				case INCH_MODE:
				case LINK_MODE:
					ModeChangeHandler(&rxbuf.cmd);
					break;
				case CONNECT:
					ConnectHandler(&rxbuf.cmd);
					break;
				case CONNECT_ACK:
					break;
				default:
					break;
			}
			rxbuf.Rxflag = 0;
		}
		LED1 = 1;
		delay_ms(500);
		LED1 = 0;
		delay_ms(500);
		Get_ADC_Value();
	}
	else
	{
		CAN_Send_Msg(NULL, 0, MAIN_BOARD, CLOSE_BOOT);
		PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
	}
}

/********************************
定时器3初始化
功能：
	主要用来做开关机按钮的定时
	
参数：
	无
	
返回值：
	无
**************************************/
#if 0
void TIM3_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_BaseStr;
	NVIC_InitTypeDef NVIC_Str;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

	NVIC_Str.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_Str.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Str.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_Str.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_Str);

	TIM_DeInit(TIM3);

	TIM_BaseStr.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_BaseStr.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_BaseStr.TIM_Period = STOP_TIME;     //两秒
	TIM_BaseStr.TIM_Prescaler = 7200 - 1;
	TIM_TimeBaseInit(TIM3, &TIM_BaseStr);

	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

//	TIM_Cmd(TIM3, ENABLE);
}
#endif
/********************************
时钟配置函数
功能：
	从停机模式下唤醒之后： 配置系统时钟允许HSE，和 pll 作为系统时钟。
	
参数：
	无
	
返回值：
	无
**************************************/
void SYSCLKConfig_STOP(void)
{
	ErrorStatus HSEStartUpStatus;
	/* Enable HSE */
	RCC_HSEConfig(RCC_HSE_ON);

	/* Wait till HSE is ready */
	HSEStartUpStatus = RCC_WaitForHSEStartUp();

	if(HSEStartUpStatus == SUCCESS)
	{

		/* Enable PLL */ 
		RCC_PLLCmd(ENABLE);

		/* Wait till PLL is ready */
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
		{
		}

		/* Select PLL as system clock source */
		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

		/* Wait till PLL is used as system clock source */
		while(RCC_GetSYSCLKSource() != 0x08)
		{
		}
	}
}


