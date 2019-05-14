
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
u8 MoveMotorStatus;  //前进电机状态
//u8 OrateMotorStatus; //转向电机状态
u32 MotorMoveTime;   //前进时间
u32 OrateMoveTime;  //转向电机时间
u8 Speed;      //前进电机速度

VolStatus BoradVol = VOL_FULL;
BoardStatus BoardSt = NORMAL;

u32 Beeptime = 0;

CommandBuf rxbuf;   //接收缓存
ConnectDev condev;  //连接设备列表

void FaultHandler(void)
{
	switch(BoardSt)
	{
		case VOL_FAULT:
		case CURRENT_FAULT:
		case ORTATE_FAULT:
		case TEMP_FAULT:
			RGBGREEN = 1;
			RGBRED = 1;
		    BEEP = 1;
			break;
		default:
			break;
	}
}

void VoltageHandler(float vol)
{
	VolStatus st;
	if(vol > 18)
	{
		BoardSt = VOL_FAULT;
	}
	else if(vol <= 18 && vol > 12)
	{
		st = VOL_FULL;
		if(BoardSt == VOL_FAULT)
		{
			BoardSt = NORMAL;
		}
		if(st != BoradVol)
		{
			LED1 = 1;
			LED2 = 1;
			LED3 = 1;
			LED4 = 1;
			BoradVol = st;
		}
	}
	else if(vol <= 12 && vol > 11)
	{
		st = VOL_LEVEL3;
		if(BoardSt == VOL_FAULT)
		{
			BoardSt = NORMAL;
		}
		if(st != BoradVol)
		{
			LED1 = 1;
			LED2 = 1;
			LED3 = 1;
			LED4 = 0;
			BoradVol = st;
		}

	}
	else if(vol <= 11 && vol > 10)
	{
		st = VOL_LEVEL2;
		if(BoardSt == VOL_FAULT)
		{
			BoardSt = NORMAL;
		}
		if(st != BoradVol)
		{
			LED1 = 1;
			LED2 = 1;
			LED3 = 0;
			LED4 = 0;
			BoradVol = st;
		}

	}
	else if(vol <= 10 && vol > 9)
	{
		st = VOL_LEVEL1;
		if(BoardSt == VOL_FAULT)
		{
			BoardSt = NORMAL;
		}
		if(st != BoradVol)
		{
			LED1 = 1;
			LED2 = 0;
			LED3 = 0;
			LED4 = 0;
			BoradVol = st;
		}

	}
	else
	{
		BoardSt = VOL_FAULT;
	}
}

void ADCHandler(void)
{
	u16 *val;
	float vol;
	float cur;
	float temp;
	val = Get_ADC_Value();
	printf("val = %d %d %d\r\n",val[0],val[1],val[2]);
	cur = val[0];
	vol = val[1] * 3.3 / 4096 * 10;  //工作电压
	temp = val[2];
	printf("vol = %f \r\n",vol);
	VoltageHandler(vol);
	
}
/********************************
电机移动时间计数函数
功能：
	时间计数
	
参数：
	无
	
返回值：
	无
**************************************/
void MotorMoveCounter(void)
{
	if(MotorMoveTime > 0)
	{
		MotorMoveTime++;
	}

	if(OrateMoveTime > 0)
	{
		OrateMoveTime++;
	}
}

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
	Speed = 0;
	MotorMoveTime = 0;
	OrateMoveTime = 0;
//	OrateMotorStatus = MOTORMOVESTOP;
	pwr_status = BOOT_STOP;  //停机开机管理变量
	pwr_time = 0; //关机时间计数
	ConnStatus = DEVINIT;
	DeviceMode = INCH_MODE;
	MoveMotorStatus = MOTORMOVESTOP;
	memset(&rxbuf,0,sizeof(rxbuf));
	memset(&condev,0,sizeof(condev));
}

/********************************
前进电机启动
功能：
	前进电机启动并设置相关速度
	
参数：
	无
	
返回值：
	无
**************************************/
void MotorMoveStart(void)
{
	TIM2->CCER |= 1 << 4;
	MotorMoveSpeedSet();
	MoveMotorStatus = MOTORMOVERUN;
}

/********************************
前进电机停止
功能：
	前进电机停止
	
参数：
	无
	
返回值：
	无
**************************************/
void MotorMoveStop(void)
{
	TIM2->CCER &=  0xffef;
	MoveMotorStatus = MOTORMOVESTOP;
}

/********************************
前进电机速度设置
功能：
	前进电机速度设置
	
参数：
	无
	
返回值：
	无
**************************************/
void MotorMoveSpeedSet(void)
{
	switch(Speed)
	{
		case SPEED0:
			TIM_SetCompare2(TIM2, SPEED0_VAL);
			break;
		case SPEED1:
			TIM_SetCompare2(TIM2, SPEED1_VAL);
			break;
		case SPEED2:
			TIM_SetCompare2(TIM2, SPEED2_VAL);
			break;
		case SPEED3:
			TIM_SetCompare2(TIM2, SPEED3_VAL);
			break;
		case SPEED4:
			TIM_SetCompare2(TIM2, SPEED4_VAL);
			break;
		case SPEED5:
			TIM_SetCompare2(TIM2, SPEED5_VAL);
			break;
		case SPEED6:
			TIM_SetCompare2(TIM2, SPEED6_VAL);
			break;
		case SPEED7:
			TIM_SetCompare2(TIM2, SPEED7_VAL);
			break;
		default:
			break;
	}
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
	DEBUGMSG("SearchDevice fail");
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
		condev.connum++;
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
		MotorMoveTime = 0;
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
		DEBUGMSG("OrtateMotorControl");
		if(dev->dev_cmd == LEFT_TURN)
		{
			if(OrtateMotorStatus == ORTATE_STATUS_LEFT)
			{
				OrateMoveTime = 1;
			}
			else if(OrtateMotorStatus == ORTATE_STATUS_STOP)
			{
				Ortate_Motor_Left();
				OrateMoveTime = 1;
			}
		}
		else if(dev->dev_cmd == RIGHT_TURN)
		{
			if(OrtateMotorStatus == ORTATE_STATUS_RIGHT)
			{
				OrateMoveTime = 1;
			}
			else if(OrtateMotorStatus == ORTATE_STATUS_STOP)
			{
				Ortate_Motor_Right();
				OrateMoveTime = 1;
			}
		}
		else
		{
			Ortate_Motor_Brate();
			OrateMoveTime = 0;
		}
		ReflashHeartBeat(dev->dev_id);
	}
}

/********************************
电机移动控制
功能：
	电机移动控制
	
参数：
	相关命令结构体
	
返回值：
	无
**************************************/
void MotorMoveControlHandler(CommandData* dev)
{
	if(SearchDevice(dev->dev_id) != 0xff)
	{
		if(dev->dev_cmd == START_MOVE)
		{
			if(MoveMotorStatus == MOTORMOVERUN)
			{
				MotorMoveTime = 1;
			}
			else
			{
				Speed = dev->data[0];
				MotorMoveSpeedSet();
				MotorMoveStart();
				MotorMoveTime = 1;
			}
		}
		else if(dev->dev_cmd == STOP_MOVE)
		{
			MotorMoveTime = 0;
			MotorMoveStop();	
		}
		else
		{
			return;		
		}
	}
}

/********************************
电机速度控制
功能：
	电机速度控制
	
参数：
	相关命令结构体
	
返回值：
	无
**************************************/
void SpeedControlHandler(CommandData* dev)
{
	if(SearchDevice(dev->dev_id) != 0xff)
	{
		Speed = dev->data[0];
		MotorMoveSpeedSet();
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
			DEBUGMSG("Control_Handler dev_cmd = %d", rxbuf.cmd.dev_cmd);
			switch(rxbuf.cmd.dev_cmd)
			{
				case HEARTBEAT:
					HeartBeatHandler(rxbuf.cmd.dev_id);
					break;
				case LEFT_TURN:
				case RIGHT_TURN:
				case ORTATE_STOP:
					OrtateMotorControl(&rxbuf.cmd);
					break;
				case START_MOVE:
				case RETREAT:
				case STOP_MOVE:
					MotorMoveControlHandler(&rxbuf.cmd);
					break;
				case SPEED_CNTR:
					SpeedControlHandler(&rxbuf.cmd);
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
//		LED1 = 1;
		if(MotorMoveTime > MAX_MOVE_TIME)
		{
			MotorMoveStop();
			MotorMoveTime = 0;
		}

		if(OrateMoveTime > MAX_MOVE_TIME)
		{
			Ortate_Motor_Brate();
			OrateMoveTime = 0;
		}
		ADCHandler();
		FaultHandler();
	}
	else
	{
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


