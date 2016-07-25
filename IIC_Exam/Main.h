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



#define DAC_DHR12RD_Address      0x40007420
#define ADC1_DR_Address    ((u32)0x4001244C)
#define PI    3.1415926


enum
{
    e_ok = 1,
    e_fail,
    e_timeout,
    e_pac_error,
    e_chksum_error
};

enum
{
	e_POWER_ON = 1,
	e_POWER_OFF,
	e_POWER_LOW,
	e_TEMP_ALARM,
	e_HeartBeat
};

#define START_MARK              ((u8)0x7E)    // 起始标志
#define END_MARK                ((u8)0x7F)    // 结束标志
#define Data_Len    1
//#define Frame_Len    (Data_Len + 6)   
#define Frame_Len   9  


typedef struct _FRAME_DATA
{
    u8 startMark;                 // 起始标志
    u8 commandID;                 // 命令编码
    u8 responseMark;              // 应答标志
    u8 dataLen;                   // 数据长度
    u8 data[Data_Len];                  // 数据域
    u8 verify;                    // 校验字
    u8 endMark;                   // 结束标志
} FrameData;






extern volatile u32 LedTimes;
extern volatile u8 RecvdPackage;
extern volatile u32 LedCnt, TimerCnt, UartRxTimeout;
extern volatile u16 Uart1RxCnt;

extern volatile u16 Uart1RxCnt, Uart2RxCnt;
extern volatile u8 Uart1RxBuffer[]; 
extern volatile u8 Uart2RxBuffer[]; 


void RCC_Configuration(void);
void GPIO_Configuration(void);
void NVIC_Configuration(void);
void SysTick_Configuration(void);
void EXIT_Configuration(void);

void Delay(vu32 nCount);
void RAM_Init(void);

void UART_Init(void);
void Timer2_Init(void);
void PulseWaveOutput_Init(void);
void Uart1Send(u8 *byte, u8 length);
void Uart2Send(u8 *byte, u8 length);
void Uart1Ack(u8 err, u8 *buf, u8 len);
void Uart1Tx(u8 commond, u8 *buf, u8 len);

void ADC1_Init(u8 ch);



void Delay100us(u32 msx);














