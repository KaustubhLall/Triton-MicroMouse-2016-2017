/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/main.h 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include <stdio.h>

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
//*****************************
#define USARTx                             USART1 
#define USARTx_IRQn                        USART1_IRQn
#define USARTx_IRQHandler                  USART1_IRQHandler
#define USARTx_CLK                         RCC_APB2Periph_USART1 
#define USARTx_CLOCKCMD                    RCC_APB2PeriphClockCmd 
#define USARTx_BAUDRATE                    9600

#define USARTx_RX_GPIO_PORT                GPIOA 
#define USARTx_RX_GPIO_CLK                 RCC_AHB1Periph_GPIOA 
#define USARTx_RX_PIN                      GPIO_Pin_10
#define USARTx_RX_AF                       GPIO_AF_USART1 
#define USARTx_RX_SOURCE                   GPIO_PinSource10   
//~~~~~~~~~~~~~~~~~
#define USARTx_TX_GPIO_PORT                GPIOA 
#define USARTx_TX_GPIO_CLK                 RCC_AHB1Periph_GPIOA 
#define USARTx_TX_PIN                      GPIO_Pin_9 
#define USARTx_TX_AF                       GPIO_AF_USART1 
#define USARTx_TX_SOURCE                   GPIO_PinSource9 
//*****************************
#define EM1_Pin      GPIO_Pin_5   //EM_L
#define EM1_GPIOX    GPIOB
#define EM1_AHBX     AHB1
#define EM1RCC() 		 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE)  

#define EM2_Pin      GPIO_Pin_4   //EM_LM
#define EM2_GPIOX    GPIOB
#define EM2_AHBX     AHB1
#define EM2RCC() 		 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE)

#define EM3_Pin      GPIO_Pin_2   //EM_LF
#define EM3_GPIOX    GPIOD
#define EM3_AHBX     AHB1
#define EM3RCC() 		 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE)

#define EM4_Pin      GPIO_Pin_10   //EM_R
#define EM4_GPIOX    GPIOC
#define EM4_AHBX     AHB1
#define EM4RCC() 		 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE)

#define EM5_Pin      GPIO_Pin_11   //EM_RM
#define EM5_GPIOX    GPIOC
#define EM5_AHBX     AHB1
#define EM5RCC() 		 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE)

#define EM6_Pin      GPIO_Pin_12   //EM_RF
#define EM6_GPIOX    GPIOC
#define EM6_AHBX     AHB1
#define EM6RCC() 		 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE)
//*****************************
#define NofCHANEL        6  
#define ADC_DR_ADDR      ((uint32_t)ADC1+0x4c)  
#define ADCx             ADC1  
#define ADC_CLK          RCC_APB2Periph_ADC1  
#define ADC_DMA_CLK      RCC_AHB1Periph_DMA2 
#define ADC_DMA_CHANNEL  DMA_Channel_0 
#define ADC_DMA_STREAM   DMA2_Stream0 

#define ADC_GPIO_CLK3    RCC_AHB1Periph_GPIOA   //RE_L   V[2]
#define ADC_GPIO_PORT3   GPIOA  
#define ADC_GPIO_PIN3    GPIO_Pin_4 
#define ADC_CHANNEL3     ADC_Channel_4

#define ADC_GPIO_CLK1    RCC_AHB1Periph_GPIOC   //RE_LM  V[0]
#define ADC_GPIO_PORT1   GPIOC  
#define ADC_GPIO_PIN1    GPIO_Pin_1 
#define ADC_CHANNEL1     ADC_Channel_11

#define ADC_GPIO_CLK4    RCC_AHB1Periph_GPIOA   //RE_LF  V[3]
#define ADC_GPIO_PORT4   GPIOA  
#define ADC_GPIO_PIN4    GPIO_Pin_5 
#define ADC_CHANNEL4     ADC_Channel_5 
 
#define ADC_GPIO_CLK5    RCC_AHB1Periph_GPIOB   //RE_R   V[4]
#define ADC_GPIO_PORT5   GPIOB  
#define ADC_GPIO_PIN5    GPIO_Pin_1 
#define ADC_CHANNEL5     ADC_Channel_9 
  
#define ADC_GPIO_CLK2    RCC_AHB1Periph_GPIOC   //RE_RM  V[1]
#define ADC_GPIO_PORT2   GPIOC  
#define ADC_GPIO_PIN2    GPIO_Pin_2 
#define ADC_CHANNEL2     ADC_Channel_12

#define ADC_GPIO_CLK6    RCC_AHB1Periph_GPIOB   //RE_RF  V[5]
#define ADC_GPIO_PORT6   GPIOB  
#define ADC_GPIO_PIN6    GPIO_Pin_0 
#define ADC_CHANNEL6     ADC_Channel_8 
//*****************************
#define TIM4_RCC()       RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE)
#define TIM4_GPIO_RCC()  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE)
#define RpwmA			       GPIO_Pin_7
#define RpwmB			       GPIO_Pin_6
#define LpwmA						 GPIO_Pin_9
#define LpwmB						 GPIO_Pin_8
#define RpwmAsource			 GPIO_PinSource7
#define RpwmBsource      GPIO_PinSource6
#define LpwmAsource      GPIO_PinSource9
#define LpwmBsource      GPIO_PinSource8
//*****************************
#define LED1_Pin      GPIO_Pin_5   //
#define LED1_GPIOX    GPIOC 
#define LED1_AHBX     AHB1
#define LED1RCC_AHB() RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE)  

#define LED2_Pin      GPIO_Pin_10   //
#define LED2_GPIOX    GPIOB 
#define LED2_AHBX     AHB1
#define LED2RCC_AHB() RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE)  

#define LED3_Pin      GPIO_Pin_11   //
#define LED3_GPIOX    GPIOB 
#define LED3_AHBX     AHB1
#define LED3RCC_AHB() RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE)  
//*****************************
#define Button_Pin      GPIO_Pin_3  
#define Button_GPIOX    GPIOA
#define Button_AHBX     AHB1
#define BUTTONRCC_AHB() RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE) 
//*****************************
extern volatile uint64_t Millis;
extern volatile uint8_t runFlag;
extern volatile uint8_t scanflag;
//************************************************************************************
#define Pi 3.14159265358979f
#define Radius 20.00f  //R_wheels in mm 
#define LwDp 1.00f //constant for omega / pwm
#define RwDp 1.00f
/* Private macro -------------------------------------------------------------*/
#define LED1_ON()  GPIO_SetBits(LED1_GPIOX, LED1_Pin)
#define LED1_OFF() GPIO_ResetBits(LED1_GPIOX, LED1_Pin)
#define LED2_ON()  GPIO_SetBits(LED2_GPIOX, LED2_Pin)
#define LED2_OFF() GPIO_ResetBits(LED2_GPIOX, LED2_Pin)
#define LED3_ON()  GPIO_SetBits(LED3_GPIOX, LED3_Pin)
#define LED3_OFF() GPIO_ResetBits(LED3_GPIOX, LED3_Pin)
//~~~~~~~~~~~
#define Button() GPIO_ReadInputDataBit(Button_GPIOX, Button_Pin)
//~~~~~~~~~~~
#define EM_L()     GPIO_SetBits(EM1_GPIOX, EM1_Pin)
#define EM_LM()		 GPIO_SetBits(EM2_GPIOX, EM2_Pin)
#define EM_LF()		 GPIO_SetBits(EM3_GPIOX, EM3_Pin)
#define EM_R()		 GPIO_SetBits(EM4_GPIOX, EM4_Pin)
#define EM_RM()    GPIO_SetBits(EM5_GPIOX, EM5_Pin)
#define EM_RF()    GPIO_SetBits(EM6_GPIOX, EM6_Pin)
#define XEM_L()     	GPIO_ResetBits(EM1_GPIOX, EM1_Pin)
#define XEM_LM()		 	GPIO_ResetBits(EM2_GPIOX, EM2_Pin)
#define XEM_LF()		 	GPIO_ResetBits(EM3_GPIOX, EM3_Pin)
#define XEM_R()		 		GPIO_ResetBits(EM4_GPIOX, EM4_Pin)
#define XEM_RM()    	GPIO_ResetBits(EM5_GPIOX, EM5_Pin)
#define XEM_RF()    	GPIO_ResetBits(EM6_GPIOX, EM6_Pin)
#define RE_L()     ADC_Value[2]
#define RE_LM()    ADC_Value[0]
#define RE_LF()    ADC_Value[3]
#define RE_R()     ADC_Value[4]
#define RE_RM()    ADC_Value[1]
#define RE_RF()    ADC_Value[5]
//~~~~~~~~~~~
#define RpwmA_CCR  TIM4->CCR1
#define RpwmB_CCR  TIM4->CCR2
#define LpwmA_CCR  TIM4->CCR3
#define LpwmB_CCR  TIM4->CCR4
//~~~~~~~~~~~~~~~~
#define ADC_SampleTime_xxxCycles ADC_SampleTime_480Cycles  //sample rate options 3 ~ 480 circles
/* Exported functions ------------------------------------------------------- */
void TimingDelay_Decrement(void);
void Millis_Increment(void);
void Delay(__IO uint32_t nTime);
void Delay_us(__IO uint64_t Time);
void ResetREnc(void);
void ResetLEnc(void);
#endif /* __MAIN_H */










/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
