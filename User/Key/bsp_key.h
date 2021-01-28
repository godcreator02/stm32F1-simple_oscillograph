#ifndef __KEY_H
#define	__KEY_H


#include "stm32f10x.h"

typedef struct
{
 	uint8_t KeyLogic;
 	uint8_t KeyONCounts;
 	uint8_t KeyOFFCounts;
}KEY_TypeDef;



//  引脚定义
#if 0

#define    KEY1_GPIO_CLK            RCC_APB2Periph_GPIOA
#define    KEY1_GPIO_PORT           GPIOA			   
#define    KEY1_GPIO_PIN		    GPIO_Pin_0

#else

#define    KEY1_GPIO_CLK            RCC_APB2Periph_GPIOA
#define    KEY1_GPIO_PORT           GPIOA			   
#define    KEY1_GPIO_PIN		    GPIO_Pin_6

#endif

#define    KEY2_GPIO_CLK            RCC_APB2Periph_GPIOC
#define    KEY2_GPIO_PORT           GPIOC		   
#define    KEY2_GPIO_PIN		    GPIO_Pin_13


 /** 按键按下标置宏
	*  按键按下为高电平，设置 KEY_ON=1， KEY_OFF=0
	*  若按键按下为低电平，把宏设置成KEY_ON=0 ，KEY_OFF=1 即可
	*/

#define    	KEY_OFF	   		1
#define    	KEY_ON	   	 	0
#define    	KEY_HOLD		2
#define		KEY_IDLE		3

#define		KEY_ERROR		10

#define		HOLD_COUNTS			50
#define 	SHAKES_COUNTS		5

void Key_GPIO_Config(void);
uint8_t Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin);


#endif /* __KEY_H */

