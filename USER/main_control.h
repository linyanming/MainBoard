
#ifndef __MAIN_CONTROL_H__
#define __MAIN_CONTROL_H__

#include "stm32f10x_pwr.h"
#include "stm32f10x_tim.h"
#include "config.h"

#define BOOT_RUN    2  //开机
#define BOOT_STOP   1  //停机
#define BOOT_INIT   0  //初始化

#define DEVINIT  0  //开机初始状态
#define DEVLOSE  1 //设备掉线
#define DEVCONN 2 //连接状态

#define STOP_TIME  2000  //长按关机时间  这里是2秒

#define MAX_CON_DEVNUM 16
#define MAX_HEART_TIME 12500 //心跳超时时间 12.5s

#define MOTORMOVERUN 1
#define MOTORMOVESTOP 0

#define MAX_MOVE_TIME 200  //电机一条命令最大转动时间 200ms

#define BEEP_ONTIME 500
#define BEEP_OFFTIME 200
#define BEEP_DELAY  500
#define BEEP_ONTIMES 2

#define RGB_SLOW_FLASH 750
#define RGB_NORMAL_FLASH 500
#define RGB_QUIK_FLASH 250

#define MAX_BIGCURRENTTIME 60000 //大电流报警时间60s

#define TEMP70  2.135 	//70度温度的电阻值
#define TEMP90  1.236   //90度温度的电阻值

#define VOLCHANGEVAL  0.3    //电压变化差值 单位：V
#define TEMPCHANGEVAL 0.05   //温度变化差值 单位：千欧
#define CURCHANGEVAL  0.5    //电流变化差值 单位：A

#define BEEPINDMAXTIME 200  //蜂鸣器指示时间200ms

#define LIMIT_CURRENT_SPD SPEED4

#define ORTATEMOTORTIME 2000 //一次最多转动两秒

#define STARTTIME 200 //开机流水灯间隔

typedef struct
{
	u8 dev_id;     //控制设备id
	u8 status;	   //连接状态
//	u8 dev_mode;   //控制设备模式 联动或者点动
	u32 heart_time; //心跳时间
}SubDevStatus;

typedef struct
{
	u8 connum;                          //已连接设备数量
	SubDevStatus dev[MAX_CON_DEVNUM];   //连接设备列表
}ConnectDev;

typedef enum
{
	VOL_FULL = 0,
	VOL_LEVEL3 = 1,
	VOL_LEVEL2 = 2,
	VOL_LEVEL1 = 3,
	VOL_NONE = 4,
}VolStatus;

typedef enum
{
	NORMAL = 0,
	MOTORRUN,
	VOL_FAULT,
	ORTATE_FAULT,
	TEMP_FAULT,
	CURRENT_FAULT60,
	CURRENT_FAULT70,
	CONNECT_FAULT,
	MOTOR_FAULT,
}BoardStatus;

typedef enum
{
	NOWARN = 0,
	VOLWARN,
	ORTATEWARN,
	TEMPWARN,
	CURRENT60WARN,
	CURRENT70WARN,
	CONNECTWARN,
	MOTORWARN,
}WarningLevel;

extern u8 pwr_status;
extern CommandBuf rxbuf;
extern u16 pwr_time;
extern u8 Speed;
extern BoardStatus BoardSt;


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
void ADCHandler(void);
void VoltageHandler(float vol);
void FaultHandler(void);
void WarningTimeCounter(void);
void WarningHandler(void);
void ConnectCheck(void);

#endif

