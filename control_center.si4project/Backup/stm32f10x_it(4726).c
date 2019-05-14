/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"

/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
#include "systick.h"
#include "key.h"
#include "pwm.h"
#include "main_control.h"
#include "can.h"
void SysTick_Handler(void)
{
	if(pwr_time >= 1)
	{
		pwr_time++;
		if(pwr_time > STOP_TIME)
		{
			pwr_status = BOOT_STOP;
			pwr_time = 0;
		}
	}
	WarningTimeCounter();
	HeartBeatCounter();
	MotorMoveCounter();
	TimingDelay_Decrement();
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/



void EXTI4_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line4) != RESET)
	{
//		delay_ms(10);
		if(KEY_PWR == 0)
		{
			if(pwr_status == BOOT_STOP)
			{
				if(PWR_GetFlagStatus(PWR_FLAG_WU) != RESET)
				{
					PWR_ClearFlag(PWR_FLAG_WU);
				}
				SYSCLKConfig_STOP();
				pwr_status = BOOT_RUN;
				CAN_Send_Msg(NULL, 0, MAIN_BOARD,START_BOOT);
//				DeviceStatusInit();
			}
			else
			{
				pwr_time = 1;
			}
		}

		if(KEY_PWR == 1)
		{
			pwr_time = 0;

		}
		
		EXTI_ClearITPendingBit(EXTI_Line4);
	}
}

#if 0
void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
	{
		pwr_status = BOOT_STOP;
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	}
}
#endif

void USB_LP_CAN1_RX0_IRQHandler(void)
{
	if(rxbuf.Rxflag == 0)
	{
		CAN_Receive_Msg(&rxbuf.cmd);
		rxbuf.Rxflag = 1;
	}
}

void CAN1_SCE_IRQHandler(void)
{
	
	if(CAN_GetITStatus(CAN1, CAN_IT_FF0) == SET)
	{
		DEBUGMSG("CAN_IT_FF0");
		CAN_ClearITPendingBit(CAN1, CAN_IT_FF0);
	}

	if(CAN_GetITStatus(CAN1, CAN_IT_ERR) == SET)
	{
		DEBUGMSG("CAN_IT_ERR %d",CAN_GetLastErrorCode(CAN1));
		
		CAN_ClearITPendingBit(CAN1, CAN_IT_ERR);
	}
}

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
