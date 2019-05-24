
#include "main_control.h"
#include <stdio.h>
#include <math.h>
#include "usart.h"
#include "led.h"
#include "systick.h"
#include "pwm.h"
#include "key.h"
#include "adc.h"
#include "iwdg.h"
#include "can.h"

__IO u8 pwr_status;  //停机开机管理变量
__IO u16 pwr_time;  //关机时间计数
u8 ConnStatus; //连接状态
u8 DeviceMode; //控制模式
u8 MoveMotorStatus;  //前进电机状态
//u8 OrateMotorStatus; //转向电机状态
__IO u16 MotorMoveTime;   //前进时间
__IO u16 OrateMoveTime;  //转向电机时间
u8 Speed;      //前进电机速度

__IO u32 BeepIndTime;  //蜂鸣器指示电机状态切换时间


VolStatus BoradVol = VOL_NONE;  //主板电量
BoardStatus BoardSt = NORMAL;   //主板状态
WarningLevel warnlv = NOWARN;   //报警等级

//u32 BigCurrenttime; //大电流报警时间
__IO u16 TempTime;   //温度报警时间
__IO u16 WpTime; //大功率报警时间

__IO u16 beepwarntime;  //蜂鸣器报警时间
__IO u32 beepwarnontime; //蜂鸣器报警持续时间
__IO u16 ledwarntime;	//LED报警时间
__IO u16 ledwarnmaxtime;  //led报警时间间隔

__IO u8 yelflashtimes;
__IO u8 redflashtimes;
__IO u8 yeltemptimes;
__IO u8 redtemptimes;

u8 OrtateMotorLock; //转向电机锁 主要用于转270度停止
__IO u16 OrtateMotorTime; //一次最多转动270度

CommandBuf rxbuf;   //接收缓存
ConnectDev condev;  //连接设备列表

float NowTemp;   //当前温度
//float NowCur;    //当前电流
float NowVol;    //当前电压
float NowWp;     //当前功率

__IO u16 StartTime;
__IO u32 SystemTime; //系统运行时间计数
void WarningTimeCounter(void)
{
	if(pwr_status == BOOT_RUN)
	{
		if(beepwarntime > 0)
			beepwarntime++;
		if(ledwarntime > 0)
			ledwarntime++;
//		if(BigCurrenttime > 0)
//			BigCurrenttime++;
		if(TempTime > 0)
			TempTime++;
		if(BeepIndTime > 0)
			BeepIndTime++;
		if(OrtateMotorTime > 0)
			OrtateMotorTime++;
		if(beepwarnontime > 0)
			beepwarnontime--;
		if(WpTime > 0)
			WpTime++;
		SystemTime++;
	}
	else if(pwr_status == BOOT_INIT)
	{
		StartTime++;
	}
}

void WarningHandler(void)
{
	if(BoardSt > NORMAL && BoardSt < CONNECT_FAULT)
	{
		if(beepwarnontime > 0)
		{
			if(beepwarntime > 0 && beepwarntime < BEEP_ONTIME)
			{
				BEEP = 1;
			}
			else if(beepwarntime >= BEEP_ONTIME && beepwarntime < (BEEP_ONTIME + BEEP_OFFTIME))
			{
				BEEP = 0;
			}
			else if(beepwarntime >= (BEEP_ONTIME + BEEP_OFFTIME) && beepwarntime < (BEEP_ONTIME * 2 + BEEP_OFFTIME))
			{
				BEEP = 1;
			}
			else if(beepwarntime >= (BEEP_ONTIME * 2 + BEEP_OFFTIME) && beepwarntime < (BEEP_ONTIME * 2 + BEEP_OFFTIME + BEEP_DELAY))
			{
				BEEP = 0;
			}
			else
			{
				beepwarntime = 1;
			}
		}

		if(yelflashtimes > 0)
		{
			if(ledwarntime > 0 && ledwarntime < ledwarnmaxtime)
			{
				RGBGREEN = 1;
				RGBRED = 1;
				RGBBLUE = 0;
			}
			else if(ledwarntime >= ledwarnmaxtime && ledwarntime < (ledwarnmaxtime * 2))
			{
				RGBBLUE = 0;
				RGBGREEN = 0;
				RGBRED = 0;
			}
			else
			{
				ledwarntime = 1;
				yelflashtimes--;
			}
		}
		else if(redflashtimes > 0)
		{
			if(ledwarntime > 0 && ledwarntime < ledwarnmaxtime)
			{
				RGBGREEN = 0;
				RGBRED = 1;
				RGBBLUE = 0;
			}
			else if(ledwarntime >= ledwarnmaxtime && ledwarntime < (ledwarnmaxtime * 2))
			{
				RGBBLUE = 0;
				RGBGREEN = 0;
				RGBRED = 0;
			}
			else
			{
				ledwarntime = 1;
				redflashtimes--;
			}
		}
		else
		{
			yelflashtimes = yeltemptimes;
			redflashtimes = redtemptimes;
		}
	}

	if(BoardSt == CONNECT_FAULT || BoardSt == MOTOR_FAULT)
	{
		if(beepwarntime > 0 && beepwarntime < BEEP_ONTIME)
		{
			BEEP = 1;
		}
		else if(beepwarntime >= BEEP_ONTIME && beepwarntime < (BEEP_ONTIME + BEEP_OFFTIME))
		{
			BEEP = 0;
		}
		else if(beepwarntime >= (BEEP_ONTIME + BEEP_OFFTIME) && beepwarntime < (BEEP_ONTIME * 2 + BEEP_OFFTIME))
		{
			BEEP = 1;
		}
		else if(beepwarntime >= (BEEP_ONTIME * 2 + BEEP_OFFTIME) && beepwarntime < (BEEP_ONTIME * 2 + BEEP_OFFTIME * 2))
		{
			BEEP = 0;
		} 
		else if(beepwarntime >= (BEEP_ONTIME * 2 + BEEP_OFFTIME * 2) && beepwarntime < (BEEP_ONTIME * 3 + BEEP_OFFTIME * 2))
		{
			BEEP = 1;
		}
		else if(beepwarntime >= (BEEP_ONTIME * 3 + BEEP_OFFTIME * 2) && beepwarntime < (BEEP_ONTIME * 3 + BEEP_OFFTIME * 2 + BEEP_DELAY))
		{
			BEEP = 0;
		}
		else
		{
			beepwarntime = 1;
		}

		RGBRED = 1;
		RGBBLUE = 0;
		RGBGREEN = 0;
	}
}

void FaultHandler(void)
{
	switch(BoardSt)
	{
#if 0
		case CURRENT_FAULT60:
			if(warnlv < CURRENT60WARN || warnlv == CURRENT70WARN)
			{
				beepwarntime = 1;
				ledwarntime = 1;
			    ledwarnmaxtime = RGB_SLOW_FLASH;
				warnlv = CURRENT60WARN;
				if(BigCurrenttime == 0)
				{
					BigCurrenttime = 1;
				}
			}
			break;
		case CURRENT_FAULT70:
			if(warnlv < CURRENT70WARN)
			{
				beepwarntime = 1;
				ledwarntime = 1;
			    ledwarnmaxtime = RGB_QUIK_FLASH;
				warnlv = CURRENT70WARN;
				if(BigCurrenttime == 0)
				{
					BigCurrenttime = 1;
				}
			}
			break;
#endif
		case WORKPOWER_FAULT:
			if(warnlv < WORKPOWERWARN)
			{
				beepwarntime = 1;
				ledwarntime = 1;
			    ledwarnmaxtime = RGB_QUIK_FLASH;
				yelflashtimes = 1;
				redflashtimes = RGB_FLASH_TIMES - yelflashtimes;
				yeltemptimes = yelflashtimes;
				redtemptimes = redflashtimes;
				if(Speed != SPEED0)
				{
					beepwarnontime = 60000;
				}
				warnlv = WORKPOWERWARN;
			}
		case VOL_FAULT:
			if(warnlv < VOLWARN)
			{
				beepwarntime = 1;
				ledwarntime = 1;
			    ledwarnmaxtime = RGB_QUIK_FLASH;
				yelflashtimes = 3;
				redflashtimes = RGB_FLASH_TIMES - yelflashtimes;
				yeltemptimes = yelflashtimes;
				redtemptimes = redflashtimes;
				beepwarnontime = 60000;
				warnlv = VOLWARN;
			}
		case ORTATE_FAULT:
			if(warnlv < ORTATEWARN)
			{
				beepwarntime = 1;
				ledwarntime = 1;
			    ledwarnmaxtime = RGB_QUIK_FLASH;
				yelflashtimes = 4;
				redflashtimes = RGB_FLASH_TIMES - yelflashtimes;
				yeltemptimes = yelflashtimes;
				redtemptimes = redflashtimes;
				beepwarnontime = 10000;
				warnlv = ORTATEWARN;
			}
			break;
		case TEMP_FAULT:
			if(warnlv < TEMPWARN)
			{
				beepwarntime = 1;
				ledwarntime = 1;
			    ledwarnmaxtime = RGB_QUIK_FLASH;
				yelflashtimes = 2;
				redflashtimes = RGB_FLASH_TIMES - yelflashtimes;
				yeltemptimes = yelflashtimes;
				redtemptimes = redflashtimes;
				beepwarnontime = 60000;
				warnlv = TEMPWARN;
			}
			break;
		case MOTOR_FAULT:
			warnlv = MOTORWARN;
			break;
		case CONNECT_FAULT:
			if(warnlv < CONNECTWARN)
				warnlv = CONNECTWARN;
			break;
		case NORMAL:
			if(MoveMotorStatus != MOTORMOVESTOP)
			{
				RGBBLUE = 1;
				RGBGREEN = 0;
			}
			else
			{
				RGBBLUE = 0;
				RGBGREEN = 1;
			}
			RGBRED = 0;
			if(warnlv != NOWARN)
				BEEP = 0;
			beepwarntime = 0;
			ledwarntime = 0;
//			BigCurrenttime = 0;
			yelflashtimes = 0;
			redflashtimes = 0;
//			yeltemptimes = yelflashtimes;
//			redtemptimes = redflashtimes;
			beepwarnontime = 0;
			TempTime = 0;
			WpTime = 0;
			warnlv = NOWARN;
		default:
			break;
	}
	WarningHandler();
}

void VoltageHandler(float vol)
{
	VolStatus st;
	if(fabs(vol - NowVol) >= VOLCHANGEVAL)
	{
		NowVol = vol;
		if(vol > 14.5)
		{
			if(BoardSt < VOL_FAULT)
			{
				BoardSt = VOL_FAULT;
			}
		}
		else if(vol <= 14.5 && vol > 12)
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
			LED1 = 0;
			LED2 = 0;
			LED3 = 0;
			LED4 = 0;
			if(BoardSt < VOL_FAULT)
			{
				BoardSt = VOL_FAULT;
			}
			BoradVol = VOL_NONE;
		}
	}
}

void WorkPowerHandler(float cur,float vol)
{
	float wp;
	wp = cur * vol;
	if(fabs(wp - NowWp) >= WPCHANGEVAL)
	{
		NowWp = wp;
		if(wp > 600)
		{
			if(WpTime == 0)
			{
				WpTime = 1;
			}

			if(WpTime > 30000)
			{
				if(BoardSt < WORKPOWER_FAULT)
				{
					BoardSt = WORKPOWER_FAULT;
				}
			}
		}
		else
		{
			if(BoardSt == WORKPOWER_FAULT)
			{
				BoardSt = NORMAL;
			}
		}
	}
}

#if 0
void CurrentHandler(float cur)
{
	if(fabs(cur - NowCur) >= CURCHANGEVAL)
	{
		NowCur = cur;
		if(cur > 60)
		{
			if(cur > 70)
			{
				if(BoardSt < CURRENT_FAULT70)
				{
					BoardSt = CURRENT_FAULT70;
				}
			}
			else
			{
				if(BoardSt <= CURRENT_FAULT70)
				{
					BoardSt = CURRENT_FAULT60;
				}
			}
			
		}
		else
		{
			if(BoardSt == CURRENT_FAULT60 || BoardSt == CURRENT_FAULT70)
			{
				BoardSt = NORMAL;
			}
		}
	}
}
#endif

void TempHandler(float temp)
{
	if(fabs(temp - NowTemp) >= TEMPCHANGEVAL)
	{
		NowTemp = temp;
		if(temp <= TEMP80)
		{
			if(TempTime == 0)
			{
				TempTime = 1;
			}

			if(TempTime > 30000)
			{
				if(BoardSt < TEMP_FAULT)
				{
					BoardSt = TEMP_FAULT;
				}
			}

		}
		else
		{
			if(BoardSt == TEMP_FAULT)
			{
				BoardSt = NORMAL;
			}
		}
	}
}

void OrtateFaultCheck(void)
{
	if(nF == 0)
	{
		if(BoardSt < ORTATE_FAULT)
		{
			BoardSt = ORTATE_FAULT;
		}
	}
	else
	{
		if(BoardSt == ORTATE_FAULT)
		{
			BoardSt = NORMAL;
		}
	}
}

void ADCHandler(void)
{
	u16 *val;
	float vol;
	float cur;
	float temp;
	if(SystemTime % 1000 == 0)
	{
		val = Get_ADC_Value();
	//	printf("val = %d %d %d\r\n",val[0],val[1],val[2]);
		cur = val[0] * 3.3 / 4096 / 20 * 1000 / 2; //工作电流
		vol = val[1] * 3.3 / 4096 * 10;  //工作电压
		temp = val[2] * 3.3 / 4096 / (3.3 - (val[2] * 3.3 / 4096)) * 5.1; //这里算出来的是热敏电阻阻值 单位：千欧
	//	printf("vol = %f cur = %f temp = %f\r\n",vol,cur,temp);
		VoltageHandler(vol);
	//	CurrentHandler(cur);
		WorkPowerHandler(cur,vol);

		TempHandler(temp);
	}
	OrtateFaultCheck();
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
	if(pwr_status == BOOT_RUN)
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
	SystemTime = 0;
	StartTime = 0;
	BeepIndTime = 0;
	OrtateMotorLock = 0;
	OrtateMotorTime = 0;
	Speed = 0;
	NowTemp = 0;
//	NowCur = 0;
	NowVol = 0;
	NowWp = 0;
	MotorMoveTime = 0;
	OrateMoveTime = 0;
//	BigCurrenttime = 0;
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
#if 1
void MotorMoveSpeedSet(void)
{
	u8 spd;
	spd = Speed;
	
	switch(spd)
	{
		case SPEED0:
			if(BoardSt == WORKPOWER_FAULT)
			{
				beepwarnontime = 0;
			}
			TIM_SetCompare2(TIM2, SPEED0_VAL);
			break;
		case SPEED1:
			if(BoardSt == WORKPOWER_FAULT || BoardSt == TEMP_FAULT)
			{
				TIM_SetCompare2(TIM2, SPEED1_VAL/2);
			}
			else
			{
				TIM_SetCompare2(TIM2, SPEED1_VAL);
			}
			break;
		case SPEED2:
			if(BoardSt == WORKPOWER_FAULT || BoardSt == TEMP_FAULT)
			{
				TIM_SetCompare2(TIM2, SPEED2_VAL/2);
			}
			else
			{
				TIM_SetCompare2(TIM2, SPEED2_VAL);
			}
			break;
		case SPEED3:
			if(BoardSt == WORKPOWER_FAULT || BoardSt == TEMP_FAULT)
			{
				TIM_SetCompare2(TIM2, SPEED3_VAL/2);
			}
			else
			{
				TIM_SetCompare2(TIM2, SPEED3_VAL);
			}
			break;
		case SPEED4:
			if(BoardSt == WORKPOWER_FAULT || BoardSt == TEMP_FAULT)
			{
				TIM_SetCompare2(TIM2, SPEED4_VAL/2);
			}
			else
			{
				TIM_SetCompare2(TIM2, SPEED4_VAL);
			}
			break;
		case SPEED5:
			if(BoardSt == WORKPOWER_FAULT || BoardSt == TEMP_FAULT)
			{
				TIM_SetCompare2(TIM2, SPEED5_VAL/2);
			}
			else
			{
				TIM_SetCompare2(TIM2, SPEED5_VAL);
			}
			break;
		case SPEED6:
			if(BoardSt == WORKPOWER_FAULT || BoardSt == TEMP_FAULT)
			{
				TIM_SetCompare2(TIM2, SPEED6_VAL/2);
			}
			else
			{
				TIM_SetCompare2(TIM2, SPEED6_VAL);
			}
			break;
		case SPEED7:
			if(BoardSt == WORKPOWER_FAULT || BoardSt == TEMP_FAULT)
			{
				TIM_SetCompare2(TIM2, SPEED7_VAL/2);
			}
			else
			{
				TIM_SetCompare2(TIM2, SPEED7_VAL);
			}
			break;
		default:
			break;
	}
}
#else
void MotorMoveSpeedSet(void)
{
	u8 spd;
	spd = Speed;
	
	switch(spd)
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
#endif

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
	if(pwr_status == BOOT_RUN)
	{
		for(u8 i = 0;i < MAX_CON_DEVNUM;i++)
		{
			if(condev.dev[i].status == DEVCONN)
			{
				condev.dev[i].heart_time++;
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
	if(condev.dev[devid].status == DEVCONN)
	{
		return devid;
	}
	DEBUGMSG("SearchDevice fail");
	return 0xff;
}

void ConnectCheck(void)
{
	for(u8 i = 0;i < MAX_CON_DEVNUM;i++)
	{
		if(condev.dev[i].heart_time > MAX_HEART_TIME)
		{
			if(condev.dev[i].status == DEVCONN)
			{
				condev.dev[i].status = DEVLOSE;
				BoardSt = CONNECT_FAULT;
				condev.connum--;
			}
		}
	}
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
//	if(SearchDevice(dev->dev_id) == 0xff)
//	{
		if(BoardSt == CONNECT_FAULT)
		{
			if(condev.dev[dev->dev_id].status == DEVLOSE)
			{
				BoardSt = NORMAL;
			}
		}
		
		if(condev.dev[dev->dev_id].status != DEVCONN)
		{
			condev.dev[dev->dev_id].dev_id = dev->dev_id;
			condev.dev[dev->dev_id].status = DEVCONN;
			condev.connum++;
		}
		
		condev.dev[dev->dev_id].heart_time = 1;
		CAN_Send_Msg(NULL, 0, condev.dev[dev->dev_id].dev_id, CONNECT_ACK);
//	}
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
		if(DeviceMode == INCH_MODE)
		{
			BeepIndTime = 1;
			BEEP = 1;
		}
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
		if(BoardSt < CONNECT_FAULT)
		{
			DEBUGMSG("OrtateMotorControl");
			
			if(dev->dev_cmd == LEFT_TURN)
			{
				if(OrtateMotorLock == 0)
				{
					if(OrtateMotorStatus == ORTATE_STATUS_LEFT)
					{
						OrateMoveTime = 1;
					}
					else if(OrtateMotorStatus == ORTATE_STATUS_STOP)
					{
						if(BoardSt == NORMAL)
						{
							BeepIndTime = 1;
							BEEP = 1;
						}
						Ortate_Motor_Left();
						OrateMoveTime = 1;
						OrtateMotorTime = 1;
					}
				}
			}
			else if(dev->dev_cmd == RIGHT_TURN)
			{
				if(OrtateMotorLock == 0)
				{
					if(OrtateMotorStatus == ORTATE_STATUS_RIGHT)
					{
						OrateMoveTime = 1;
					}
					else if(OrtateMotorStatus == ORTATE_STATUS_STOP)
					{
						if(BoardSt == NORMAL)
						{
							BeepIndTime = 1;
							BEEP = 1;
						}

						Ortate_Motor_Right();
						OrateMoveTime = 1;
						OrtateMotorTime = 1;
					}
				}
			}
			else
			{	
/*				if(BoardSt == NORMAL)
				{
					BeepIndTime = 1;
					BEEP = 1;
				}  */

				Ortate_Motor_Brate();
				OrtateMotorLock = 0;
				OrtateMotorTime = 0;
				OrateMoveTime = 0;
			}
			ReflashHeartBeat(dev->dev_id);
		}
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
		if(BoardSt <= WORKPOWER_FAULT)
		{
			if(dev->dev_cmd == START_MOVE)
			{
				if(MoveMotorStatus == MOTORMOVERUN)
				{
					MotorMoveTime = 1;
				}
				else
				{
					if(BoardSt == NORMAL)
					{
						BeepIndTime = 1;
						BEEP = 1;
					}

					Speed = dev->data[0];
					MotorMoveSpeedSet();
					MotorMoveStart();
					MotorMoveTime = 1;
				}
			}
			else if(dev->dev_cmd == STOP_MOVE)
			{
/*				if(BoardSt == NORMAL)
				{
					BeepIndTime = 1;
					BEEP = 1;
				}*/

				MotorMoveTime = 0;
				MotorMoveStop();	
			}
			else
			{
				return;		
			}
		}
		ReflashHeartBeat(dev->dev_id);
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
		if(Speed != dev->data[0])
		{
			if(BoardSt == NORMAL)
			{
				BeepIndTime = 1;
				BEEP = 1;
			}

			Speed = dev->data[0];
			MotorMoveSpeedSet();
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

#if 0		
		if(BigCurrenttime > MAX_BIGCURRENTTIME)
		{
			BoardSt = MOTOR_FAULT;
//			BigCurrenttime = 0;
		}
#endif
		if(MotorMoveTime > MAX_MOVE_TIME)
		{
			MotorMoveStop();
			MotorMoveTime = 0;
		}

		if(BeepIndTime > BEEPINDMAXTIME)
		{
			BeepIndTime = 0;
			BEEP = 0;
		}

		if(OrateMoveTime > MAX_MOVE_TIME)
		{
			Ortate_Motor_Brate();
//			OrtateMotorLock = 0;
			OrateMoveTime = 0;
			OrtateMotorTime = 0;
		}

		if(OrtateMotorTime > ORTATEMOTORTIME)
		{
			Ortate_Motor_Brate();
			OrateMoveTime = 0;
			OrtateMotorLock = 1;
			OrtateMotorTime = 0;
		}
		ConnectCheck();
		ADCHandler();
		FaultHandler();
	}
	else if(pwr_status == BOOT_INIT)
	{
//		StartTime = 0;
		RGBBLUE = 1;
		RGBGREEN = 1;
		RGBRED = 1;
		if(StartTime <= STARTTIME)
		{
			LED1 = 1;
			LED2 = 0;
			LED3 = 0;
			LED4 = 0;
		}
		else if(StartTime > STARTTIME && StartTime <= (STARTTIME * 2))
		{
			LED1 = 0;
			LED2 = 1;
			LED3 = 0;
			LED4 = 0;
		}
		else if(StartTime > (STARTTIME * 2) && StartTime <= (STARTTIME * 3))
		{
			LED1 = 0;
			LED2 = 0;
			LED3 = 1;
			LED4 = 0;
		}
		else if(StartTime > (STARTTIME * 3) && StartTime <= (STARTTIME * 4))
		{
			LED1 = 0;
			LED2 = 0;
			LED3 = 0;
			LED4 = 1;
		}
		else
		{
			LED1 = 0;
			LED2 = 0;
			LED3 = 0;
			LED4 = 0;
			
			pwr_status = BOOT_RUN;
			StartTime = 0;
			CAN_Send_Msg(NULL, 0, MAIN_BOARD,START_BOOT);
		}
	}
	else
	{
		SystemTime = 0;
		MotorMoveStop();
		Ortate_Motor_Brate();
		BoardSt = NORMAL;
		warnlv = NOWARN;
		NowVol = 0;
//		NowCur = 0;
		NowTemp = 0;
		NowWp = 0;
		beepwarnontime = 0;
		yelflashtimes = 0;
		redflashtimes = 0;
		MotorMoveTime = 0;
		OrtateMotorLock = 0;
		OrtateMotorTime = 0;
		OrateMoveTime = 0;
		BeepIndTime = 0;
		BEEP = 0;
		DeviceMode = INCH_MODE;
		CAN_Send_Msg(NULL, 0, MAIN_BOARD, CLOSE_BOOT);
		while(CAN_GetFlagStatus(CAN1, CAN_FLAG_RQCP0) == RESET);
		BoradVol = VOL_NONE;
		
		Led_Reset();
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


