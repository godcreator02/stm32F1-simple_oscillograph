#ifndef __BSP_TPAD_H
#define __BSP_TPAD_H


#include "stm32f10x.h"
#include "./Systick/bsp_SysTick.h"
#include "./usart/bsp_usart.h"

/************通用定时器TIM参数定义，只限TIM2、3、4、5************/
// 当使用不同的定时器的时候，对应的GPIO是不一样的，这点要注意
// 我们这里默认使用TIM5

#define            TPAD_TIM                   TIM5
#define            TPAD_TIM_APBxClock_FUN     RCC_APB1PeriphClockCmd
#define            TPAD_TIM_CLK               RCC_APB1Periph_TIM5
#define            TPAD_TIM_Period            0XFFFF
#define            TPAD_TIM_Prescaler         71

// TIM 输入捕获通道GPIO相关宏定义
#define            TPAD_TIM_CH_GPIO_CLK       RCC_APB2Periph_GPIOA
#define            TPAD_TIM_CH_PORT           GPIOA
#define            TPAD_TIM_CH_PIN            GPIO_Pin_1
#define            TPAD_TIM_CHANNEL_x         TIM_Channel_2

// 中断相关宏定义
#define            TPAD_TIM_IT_CCx            TIM_IT_CC2
#define            TPAD_TIM_IRQ               TIM5_IRQn
#define            TPAD_TIM_INT_FUN           TIM5_IRQHandler

// 获取捕获寄存器值函数宏定义
#define            TPAD_TIM_GetCapturex_FUN                 TIM_GetCapture2
// 捕获信号极性函数宏定义
#define            TPAD_TIM_OCxPolarityConfig_FUN           TIM_OC2PolarityConfig

// 电容按键被按下的时候门限值，需要根据不同的硬件实际测试，
// 减小这个门限值可以提高响应速度
#define            TPAD_GATE_VAL              100
// 电容按键空载的时候的最大和最小的充电时间，不同的硬件不一样，霸道稳定在218
#define            TPAD_DEFAULT_VAL_MIN       210
#define            TPAD_DEFAULT_VAL_MAX       230

#define            TPAD_ON                     1
#define            TPAD_OFF                    0

/**************************函数声明********************************/
uint8_t TPAD_Init(void);
uint8_t TPAD_Scan(void);

#endif	/* __BSP_TPAD_H */


