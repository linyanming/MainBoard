
#ifndef __MAIN_CONTROL_H__
#define __MAIN_CONTROL_H__

#include "stm32f10x_pwr.h"
#include "stm32f10x_tim.h"
#include "config.h"

#define BOOT_RUN    0  //开机
#define BOOT_STOP   1  //停机

#define DEVINIT  1  //开机初始状态
#define DEVLOSE  2 //设备掉线
#define DEVCONN 3 //连接状态

#define STOP_TIME  2000  //长按关机时间  这里是2秒

#define MAX_CON_DEVNUM 16
#define MAX_HEART_TIME 7500 //心跳超时时间 7.5s

#define MOTORMOVERUN 1
#define MOTORMOVESTOP 0

extern u8 pwr_status;
extern CommandBuf rxbuf;
extern u16 pwr_time;
extern u8 Speed;

typedef struct
{
	u8 dev_id;     //控制设备id
//	u8 dev_mode;   //控制设备模式 联动或者点动
	u32 heart_time; //心跳时间
}SubDevStatus;

typedef struct
{
	u8 connum;                          //已连接设备数量
	SubDevStatus dev[MAX_CON_DEVNUM];   //连接设备列表
}ConnectDev;


void TIM3_Init(void);
void SYSCLKConfig_STOP(void);
void Control_Handler(void);
void HeartBeatCounter(void);
void DeviceStatusInit(void);
void MotorMoveStart(void);
void MotorMoveStop(void);
void MotorMoveSpeedSet(void);
u8 SearchDevice(u8 devid);
void ReflashHeartBeat(u8 devid);
void HeartBeatHandler(u8 devid);
void ConnectHandler(CommandData* dev);
void OrtateMotorControl(CommandData* dev);
void SpeedControlHandler(CommandData* dev);
void MotorMoveControlHandler(CommandData* dev);
void ModeChangeHandler(CommandData* dev);
void MotorMoveCounter(void);

#endif

