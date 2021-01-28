/**
  ******************************************************************************
  * @file    bsp_key.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   按键应用bsp（扫描模式）
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火 F103-霸道 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 
  
#include "bsp_key.h"  
#include "bsp_usart.h"

/**
  * @brief  配置按键用到的I/O口
  * @param  无
  * @retval 无
  */
void Key_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure = {0};
	
	/*开启按键端口的时钟*/
	RCC_APB2PeriphClockCmd(KEY1_GPIO_CLK|KEY2_GPIO_CLK,ENABLE);
	
	//选择按键的引脚
	GPIO_InitStructure.GPIO_Pin = KEY1_GPIO_PIN; 
	// 设置按键的引脚为浮空输入
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	//使用结构体初始化按键
	GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure);
	
	//选择按键的引脚
	GPIO_InitStructure.GPIO_Pin = KEY2_GPIO_PIN; 
	//设置按键的引脚为浮空输入
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	//使用结构体初始化按键
	GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStructure);	
}

 /*
 * 函数名：Key_Scan
 * 描述  ：检测是否有按键按下
 * 输入  ：GPIOx：x 可以是 A，B，C，D或者 E
 *		     GPIO_Pin：待读取的端口位 	
 * 输出  ：KEY_OFF(没按下按键)、KEY_ON（按下按键）
 */

static uint8_t Key1Flag = 0;
static uint8_t Key2Flag = 0;

uint8_t Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin)
{	
	uint8_t* KeyFlag;

	switch ((uint32_t)GPIOx)
	{
	case ((uint32_t)KEY1_GPIO_PORT):
		switch (GPIO_Pin)
		{
		case KEY1_GPIO_PIN:
			KeyFlag = &Key1Flag;
			break;
		
		default:
			printf("error: GPIO port pin not match\r\n");
			return KEY_OFF;
			
		}
		break;

	case ((uint32_t)KEY2_GPIO_PORT):
		switch (GPIO_Pin)
		{
		case KEY2_GPIO_PIN:
			KeyFlag = &Key2Flag;
			break;
		
		default:
			printf("error: GPIO port pin not match\r\n");
			return KEY_OFF;
			
		}
		break;

	default:
		printf("error: key do not exist\r\n");
		return KEY_OFF;
	}

	/*检测是否有按键按下 */
	if( !(*KeyFlag) && (GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON ))  
	{
		*KeyFlag = 1;
		return 	KEY_ON;	 		
	}
	else if( (*KeyFlag) && (GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_OFF))
	{
		*KeyFlag = 0;
		return 	KEY_OFF;
	}
	else
	{
		return KEY_OFF;
	}
		
}
/*********************************************END OF FILE**********************/
