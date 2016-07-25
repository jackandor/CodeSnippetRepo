/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.4.0
  * @date    10/15/2010
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2010 STMicroelectronics</center></h2>
  */ 

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include "Main.h"
#include <string.h>




void USART1_IRQHandler(void)
{

  if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
  {

  }
}

void USART2_IRQHandler(void)
{
  if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
  {
        if(Uart2RxCnt == Frame_Len)
        {
            if(USART_ReceiveData(USART2))
            {
            }
            return;
        }
      
      Uart2RxBuffer[Uart2RxCnt] = USART_ReceiveData(USART2);      
      Uart2RxCnt++;
      UartRxTimeout = 0;
      
      if(Uart2RxCnt < Frame_Len)
      {
          UartRxTimeout = 1000;  /* 超过100ms没有收到下一个数据就清除缓存  */
          return;
      }
      
  //    RecvdPackage = 1;

  }
  
}

// 20ms per period
void SysTick_Handler(void)
{    
}

// 100us period
void TIM2_IRQHandler(void)
{
    static u8 LED_status = 1;
    
    TIM_ClearITPendingBit(TIM2, TIM_FLAG_Update);

    if(TimerCnt > 0)
    {
        TimerCnt--;
    }

    if(UartRxTimeout)
    {
        UartRxTimeout--;
        if(UartRxTimeout == 0)
        {
         //   Uart1RxCnt = 0;
            Uart2RxCnt = 0;
        }
    }

    LedCnt++;
    if(LedCnt > LedTimes) // 500ms
    {
        LedCnt = 0;
        LED_status ^= 1;
        if(LED_status)
        {
            GPIO_ResetBits(GPIOA, GPIO_Pin_0);
        }
        else
        {
            GPIO_SetBits(GPIOA, GPIO_Pin_0);
        }
    }

}


void EXTI9_5_IRQHandler(void)
{
    if(EXTI_GetITStatus(EXTI_Line9) != RESET)
    {
      // Clear the EXTI line 9 pending bit 
      EXTI_ClearITPendingBit(EXTI_Line9);
    } 

}

/**
  * @brief   This function handles NMI exception.
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


/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

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


/******************* (C) COPYRIGHT 2010 STMicroelectronics *****END OF FILE****/
