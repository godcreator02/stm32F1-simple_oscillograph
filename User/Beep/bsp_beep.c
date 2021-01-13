#include "./BEEP/bsp_beep.h"

void Beep_GPIO_Config(void)
{		
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/*开启连接蜂鸣器的GPIO的外设时钟*/
	BEEP_GPIO_APBxClock_FUN(BEEP_GPIO_CLK, ENABLE);
	/*选择要控制的GPIO引脚*/															   
	GPIO_InitStructure.GPIO_Pin = BEEP_PIN;
	/*设置引脚模式为通用推挽输出*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	/*设置引脚速率为50MHz */   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	/*调用库函数，初始化GPIO*/
	GPIO_Init(BEEP_PORT, & GPIO_InitStructure);

	/* 默认关闭蜂鸣器*/
	BEEP_OFF ();	
}

void Beep_Init(void)
{		
	Beep_GPIO_Config ();
	
	/* 默认关闭蜂鸣器*/
	BEEP_OFF ();
}


/*********************************************END OF FILE**********************/
