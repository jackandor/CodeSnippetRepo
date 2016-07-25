/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : main.c
* Author             : MCD Application Team
* Version            : V2.0.1
* Date               : 06/13/2008
* Description        : Main program body.
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <math.h>
#include <string.h>
#include "Main.h"

/* Includes ------------------------------------------------------------------*/
#include "i2c_ee.h"



volatile u32 LedTimes;

volatile u8 RecvdPackage;
volatile u32 LedCnt, TimerCnt, UartRxTimeout;
volatile u16 Uart1RxCnt, Uart2RxCnt;

u8 Uart1TxBuffer[Frame_Len];
volatile u8 Uart1RxBuffer[Frame_Len];
volatile u8 Uart2TxBuffer[40];
volatile u8 Uart2RxBuffer[40];

u8 reg_init[40];

int main(void)
{
    u8 checksum;
    u32 i;

#ifdef DEBUG
  debug();
#endif

  RAM_Init();

  /* System Clocks Configuration */
  RCC_Configuration();   

  /* GPIO configuration */
  GPIO_Configuration();

  /* NVIC Configuration */
  NVIC_Configuration();

  EXIT_Configuration();

  Timer2_Init();


//  SysTick_Configuration();

  UART_Init();

//    Delay100us(10 * 500);


    /* Initialize the I2C EEPROM driver ----------------------------------------*/
    I2C_EE_Init();  


    EEPROM_ADDRESS = EEPROM_Block0_ADDRESS;
    reg_init[0] = 0;  // disable trickle charger
    reg_init[1] = 0;  /* clear the OSF, WF bits */
    reg_init[2] = 0;  /* enable osc, disable WDE, WD/RST */
    I2C_EE_PageWrite(reg_init, 0x0a, 3); 



    while (1)
    {
        while(Uart2RxCnt != Frame_Len);

        
        if(Uart2RxBuffer[0] == 1 && Uart2RxBuffer[8] == 'E')
        {
            LedTimes = 500;
            memcpy((char *)&reg_init[1], (char *)&Uart2RxBuffer[1], 7);
            reg_init[0] = 0;  /* default for msec register */
         //   reg_init[1] = 0x30;  // sec
         //   reg_init[2] = 0x15;  // min
         //   reg_init[3] = 0x13 & 0x3f;  // hour
            reg_init[3]  &= 0x3f;  // hour
        //    reg_init[4] = 0x05;  // day  1-7
        //    reg_init[5] = 0x24;  // date 1-31
        //    reg_init[6] = 0x04;  // month 1-12
        //    reg_init[7] = 0x15;  // year 0-99

            
            I2C_EE_PageWrite(reg_init, 0x00, 8); 

            memcpy((char *)&Uart2TxBuffer[0], (char *)&Uart2RxBuffer[0], Frame_Len);
            Uart2Send(Uart2TxBuffer, Frame_Len);
        }
        else if(Uart2RxBuffer[0] == 2 && Uart2RxBuffer[8] == 'E')
        {
            
            I2C_EE_BufferRead(&Uart2TxBuffer[1], 1, 7); 
            Uart2TxBuffer[0] = 2;
            Uart2TxBuffer[8] = 'E';

            Uart2Send(Uart2TxBuffer, Frame_Len);
        }

        
        Uart2RxCnt = 0;
    //    RecvdPackage = 0;


    }

}

/*******************************************************************************
* Function Name  : RCC_Configuration
* Description    : Configures the different system clocks.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RCC_Configuration(void)
{   
    ErrorStatus HSEStartUpStatus;
    
  /* RCC system reset(for debug purpose) */
  RCC_DeInit();

  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);

  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();

  if(HSEStartUpStatus == SUCCESS)
  {
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);

    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);
 	
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
  
    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1); 

    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);

    /* ADCCLK */
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);  // 72/6=12MHz 


    /* PLLCLK = 8MHz * 9 = 72 MHz */
    RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);

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


/* Enable peripheral clocks --------------------------------------------------*/
  /* DMA clock enable */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2 | RCC_AHBPeriph_DMA1, ENABLE);
  /* AFIO and GPIOA Periph clock enable */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | 
                                                RCC_APB2Periph_GPIOA | 
                                                RCC_APB2Periph_GPIOF |
                                                RCC_APB2Periph_GPIOB |
                                                RCC_APB2Periph_USART1 |
                                                RCC_APB2Periph_TIM8 |
                                                RCC_APB2Periph_TIM1 |
                                                RCC_APB2Periph_ADC1 | 
                                                RCC_APB2Periph_GPIOC |
                                                RCC_APB2Periph_GPIOD, 
                                                ENABLE);
  /* DAC Periph clock enable */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC | 
                                                    RCC_APB1Periph_TIM2 |
                                                  RCC_APB1Periph_USART2, 
                                                  ENABLE);
                                                  
  /* Enable peripheral clocks --------------------------------------------------*/
    /* GPIOB Periph clock enable */
 //   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    /* I2C1 Periph clock enable */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

}

/*******************************************************************************
* Function Name  : GPIO_Configuration
* Description    : Configures the different GPIO ports.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//LED
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);

    
	/* Configure USART1 & UART2 Rx (PA.10) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* Configure USART1 & UART2 Tx (PA.09) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
}

/*******************************************************************************
* Function Name  : NVIC_Configuration
* Description    : Configures Vector Table base location.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void NVIC_Configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    
#ifdef  VECT_TAB_RAM  
  /* Set the Vector Table base location at 0x20000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0); 
#else  /* VECT_TAB_FLASH  */
  /* Set the Vector Table base location at 0x08000000 */ 
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);   
#endif

    /* Configure the NVIC Preemption Priority Bits */  
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    /* Enable the USART1 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	  
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* Enable the USART2 Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

   NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn; 
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; 
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;   
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;     
   NVIC_Init(&NVIC_InitStructure);                 
}

void SysTick_Configuration(void)
{
    if(SysTick_Config(SystemCoreClock/50)) // 1s/50=20ms
    { 
      /* Capture error */ 
      while (1);
    }
}

void EXIT_Configuration(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;
 
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource9); 
    EXTI_InitStructure.EXTI_Line = EXTI_Line9; 
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising; 
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);   
}


void RAM_Init(void)
{
    int i;
    
    Uart2RxCnt = 0;

    RecvdPackage = 0;
    LedCnt = 0;
    TimerCnt = 0;
    UartRxTimeout = 0;
    LedTimes = 5000;




}



void UART_Init(void)
{
    USART_InitTypeDef USART_InitStructure;
    
  /* USART1 and USART2 configuration ------------------------------------------------------*/
    USART_InitStructure.USART_BaudRate = 9600;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  
    /* Configure USART1 */
    USART_Init(USART1, &USART_InitStructure);
    /* Configure USART2 */
    USART_Init(USART2, &USART_InitStructure);
    
    /* Enable USART1 Receive and Transmit interrupts */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
//    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
  
    /* Enable USART2 Receive and Transmit interrupts */
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
 //   USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
  
    /* Enable the USART1 */
    USART_Cmd(USART1, ENABLE);
    /* Enable the USART2 */
    USART_Cmd(USART2, ENABLE);

}


/* TIM2与DAC输出产生冲突 */
void Timer2_Init(void)
{
    TIM_TimeBaseInitTypeDef    TIM2_TimeBaseStructure;
    TIM_TypeDef *tmx = TIM2;

    
  TIM_DeInit(tmx);
  TIM_TimeBaseStructInit(&TIM2_TimeBaseStructure); 


  TIM2_TimeBaseStructure.TIM_Period = 20 -1;		// 72M/36/20 = 100000HZ 																	
  TIM2_TimeBaseStructure.TIM_Prescaler = 360 - 1;			
  TIM2_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 			
  TIM2_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 		

  TIM_TimeBaseInit(tmx, &TIM2_TimeBaseStructure);

  TIM_ClearFlag(tmx, TIM_FLAG_Update);							
  TIM_ITConfig(tmx,TIM_IT_Update,ENABLE);
  TIM_Cmd(tmx, ENABLE);		

}

void Uart1Send(u8 *byte, u8 length)
{
    u8 i;
    
    for(i = 0; i < length; i++)
    {
        while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
        USART_SendData(USART1, byte[i]);
    }
}

void Uart2Send(u8 *byte, u8 length)
{
    u8 i;
    
    for(i = 0; i < length; i++)
    {
        while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
        USART_SendData(USART2, byte[i]);
    }
}


void Uart1Ack(u8 err, u8 *buf, u8 len)
{
    FrameData cmd;
    u32 i;

    cmd.startMark = START_MARK;
//    cmd.commandID = STEP;
    cmd.endMark = END_MARK;
    cmd.responseMark = err;
    cmd.dataLen = len;
    memset(&cmd.data[0], 0, Data_Len);
    if(len > 0)
    {
        memcpy(&cmd.data[0], buf, len);
    }
    cmd.verify = 0;
    cmd.verify += cmd.startMark;
    cmd.verify += cmd.commandID;
    cmd.verify += cmd.responseMark;
    cmd.verify += cmd.dataLen;
    for(i = 0; i < Data_Len; i++)
    {
        cmd.verify += cmd.data[i];
    }
    memcpy(Uart1TxBuffer, &cmd.startMark, Frame_Len);
    Uart1Send(&Uart1TxBuffer[0], Frame_Len);
}

void Uart1Tx(u8 commond, u8 *buf, u8 len)
{
    FrameData cmd;
    u32 i;

    cmd.startMark = START_MARK;
    cmd.commandID = commond;
    cmd.endMark = END_MARK;
    cmd.responseMark = 0;
    cmd.dataLen = len;
    memset(&cmd.data[0], 0, Data_Len);
    if(len > 0)
    {
        memcpy(&cmd.data[0], buf, len);
    }
    cmd.verify = 0;
    cmd.verify += cmd.startMark;
    cmd.verify += cmd.commandID;
    cmd.verify += cmd.responseMark;
    cmd.verify += cmd.dataLen;
    for(i = 0; i < Data_Len; i++)
    {
        cmd.verify += cmd.data[i];
    }
    memcpy(Uart1TxBuffer, &cmd.startMark, Frame_Len);
    Uart1Send(&Uart1TxBuffer[0], Frame_Len);
}





void Delay100us(u32 msx)
{		
    TimerCnt = msx;
    while(TimerCnt != 0);		
}




#ifdef  DEBUG
/*******************************************************************************
* Function Name  : assert_failed
* Description    : Reports the name of the source file and the source line number
*                  where the assert_param error has occurred.
* Input          : - file: pointer to the source file name
*                  - line: assert_param error line source number
* Output         : None
* Return         : None
*******************************************************************************/
void assert_failed(u8* file, u32 line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/

