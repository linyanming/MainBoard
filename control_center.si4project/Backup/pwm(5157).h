
#ifndef __PWM_H__
#define __PWM_H__

#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_tim.h"

#define PWM_FRE 5000  //5Khz 频率

#define PWM_PERIOD (4800 - 1)  //PWM计数值
#define PRESCALER (PWM_FRE * (PWM_PERIOD + 1))  //时钟24Mhz

#define PWM_PRESCALER ((u16)(SystemCoreClock / PRESCALER) - 1)  //分频系数

#define ORTATE_PWM_PRE 80  //转向电机PWM占空比
#define ORTATE_PWM_PULSE ((ORTATE_PWM_PRE * (PWM_PERIOD + 1) / 100)) //转向电机占空比值

#define ORTATE_STATUS_LEFT 3
#define ORTATE_STATUS_RIGHT 2
#define ORTATE_STATUS_STOP 1

extern u8 OrtateMotorStatus;


void Motor_Pwm_Init(void);
void Ortate_Motor_Left(void);
void Ortate_Motor_Right(void);
void Ortate_Motor_Coast(void);
void Ortate_Motor_Brate(void);


#endif

