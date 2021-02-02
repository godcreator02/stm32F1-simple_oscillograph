#ifndef __ADC_H
#define	__ADC_H


#include "stm32f10x.h"

// 注意：用作ADC采集的IO必须没有复用，否则采集电压会有影响
/********************ADC1输入通道（引脚）配置**************************/
#define    ADCx                          ADC1
#define    ADC_APBxClock_FUN             RCC_APB2PeriphClockCmd
#define    ADC_CLK                       RCC_APB2Periph_ADC1

#define    ADC_GPIO_APBxClock_FUN        RCC_APB2PeriphClockCmd
#define    ADC_GPIO_CLK                  RCC_APB2Periph_GPIOC  
#define    ADC_PORT                      GPIOC

// PC1-通道11 独立IO
#define    ADC_PIN                       GPIO_Pin_5
#define    ADC_CHANNEL                   ADC_Channel_15
#define    ADC_DMA_CHANNEL               DMA1_Channel1


#define    ADC_DataSize                  2000
extern __IO uint16_t ADC_ConvertedValue[ADC_DataSize];


void ADCx_Init(void);
void SetADCSampleRate(uint32_t SampleRate);

#endif /* __ADC_H */

