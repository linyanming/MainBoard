

#ifndef __KEY_H__
#define __KEY_H__

#include "stm32f0xx_gpio.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_exti.h"
#include "config.h"

#define KEYINIT    0  //按键初始
#define KEYPRESS   1  //按键按下
#define KEYRELEASE 2  //按键弹起

extern u8 KeyModeStatus;
extern u8 KeyPressStatus;
extern u8 KeyLeftStatus;
extern u8 KeyRightStatus;

extern u8 KeyStatus;

typedef enum
{
	KEYNONE = 0,    //无按键按下
	KMPRESS,        //按键mode按下
	KMRELEASE,      //按键mode释放
	KPPRESS,        //press按键按下
	KPRELEASE,      //press按键释放
	KLPRESS,        //left按键按下
	KLRELEASE,      //left按键释放
	KRPRESS,        //right按键按下
	KRRELEASE,      //right按键按下
}KeyEvent;

//按键引脚数值读取
#define KEY_MODE    PAin(1)
#define KEY_LEFT    PAin(0)
#define KEY_PRESS   PAin(6)
#define KEY_RIGHT   PAin(7)

typedef enum
{
	K_MODE = 0,
	K_LEFT,
	K_RIGHT,
	K_PRESS,
}KeyTouch;

void Key_Init(void);
void Key_EXTI_Init(void);
void KeyStatusCheck(KeyEvent key);

#endif

