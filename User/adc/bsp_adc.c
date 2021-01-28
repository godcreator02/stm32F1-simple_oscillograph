#include "bsp_adc.h"


__IO uint16_t ADC_ConvertedValue[ADC_DataSize];





/**
  * @brief  ADC GPIO 初始化
  * @param  无
  * @retval 无
  */
static void ADCx_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// 打开 ADC IO端口时钟
	ADC_GPIO_APBxClock_FUN ( ADC_GPIO_CLK, ENABLE );
	
	// 配置 ADC IO 引脚模式
	GPIO_InitStructure.GPIO_Pin = ADC_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	
	// 初始化 ADC IO
	GPIO_Init(ADC_PORT, &GPIO_InitStructure);				
}

/**
  * @brief  TIM3 初始化
  * @param  无
  * @retval 无
  */

static void TIM3_Config(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

    /* 使能TIM3时钟，TIM2CLK 为72M */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    /* TIM3基本定时器配置 */
    // TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
    TIM_TimeBaseStructure.TIM_Period = (12 - 1);              //定时周期 72 /12 =6
    TIM_TimeBaseStructure.TIM_Prescaler = (6-1);     //预分频 72M / 6 = 1M
    TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;              //时钟分频系数
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //向上计数模式
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    /* 配置TIM3触发源 */
    TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);

    /* 使能TIM3 */
    TIM_Cmd(TIM3, ENABLE);
}

/**
  * @brief  配置ADC工作模式
  * @param  无
  * @retval 无
  */
static void ADCx_Mode_Config(void)
{
	DMA_InitTypeDef DMA_InitStructure;
	ADC_InitTypeDef ADC_InitStructure;
	
	// 打开DMA时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	// 打开ADC时钟
	ADC_APBxClock_FUN ( ADC_CLK, ENABLE );
	
	// 复位DMA控制器
	DMA_DeInit(ADC_DMA_CHANNEL);
	
	// 配置 DMA 初始化结构体
	// 外设基址为：ADC 数据寄存器地址
	DMA_InitStructure.DMA_PeripheralBaseAddr = ( uint32_t ) ( & ( ADCx->DR ) );
	
	// 存储器地址，实际上就是一个内部SRAM的变量
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADC_ConvertedValue;
	
	// 数据源来自外设
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	
	// 缓冲区大小为1，缓冲区的大小应该等于存储器的大小
	DMA_InitStructure.DMA_BufferSize = ADC_DataSize;
	
	// 外设寄存器只有一个，地址不用递增
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;

	// 存储器地址递增
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; 
	
	// 外设数据大小为半字，即两个字节
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	
	// 存储器数据大小也为半字，跟外设数据大小相同
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	
	// 循环传输模式
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	

	// DMA 传输通道优先级为高，当使用一个DMA通道时，优先级设置不影响
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	
	// 禁止存储器到存储器模式，因为是从外设到存储器
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	
	// 初始化DMA
	DMA_Init(ADC_DMA_CHANNEL, &DMA_InitStructure);
	
	// 使能 DMA 通道
	DMA_Cmd(ADC_DMA_CHANNEL , ENABLE);
	
	// ADC 模式配置
	// 只使用一个ADC，属于单模式
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	
	// 禁止扫描模式，多通道才要，单通道不需要
	ADC_InitStructure.ADC_ScanConvMode = DISABLE ; 

	// 单次转换模式，用tim3触发
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;

	// 使用TIM3的TRGO作为触发源
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;

	// 转换结果右对齐
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	
	// 转换通道1个
	ADC_InitStructure.ADC_NbrOfChannel = 1;	
		
	// 初始化ADC
	ADC_Init(ADCx, &ADC_InitStructure);
	
	// 配置ADC时钟为PCLK2的2分频，即32MHz
	RCC_ADCCLKConfig(RCC_PCLK2_Div2); 
	
	// 配置 ADC 通道转换顺序为1，第一个转换，采样时间为1.5个时钟周期
	ADC_RegularChannelConfig(ADCx, ADC_CHANNEL, 1, ADC_SampleTime_1Cycles5);
	
	// 使能ADC DMA 请求
	ADC_DMACmd(ADCx, ENABLE);
	
	// 开启ADC ，并开始转换
	ADC_Cmd(ADCx, ENABLE);
	
	// 初始化ADC 校准寄存器  
	ADC_ResetCalibration(ADCx);
	// 等待校准寄存器初始化完成
	while(ADC_GetResetCalibrationStatus(ADCx));
	
	// ADC开始校准
	ADC_StartCalibration(ADCx);
	// 等待校准完成
	while(ADC_GetCalibrationStatus(ADCx));
	
    ADC_ExternalTrigConvCmd(ADC1,ENABLE);

}

/**
  * @brief  ADC初始化
  * @param  无
  * @retval 无
  */
void ADCx_Init(void)
{
	ADCx_GPIO_Config();
	ADCx_Mode_Config();
    TIM3_Config();
}
/*********************************************END OF FILE**********************/
