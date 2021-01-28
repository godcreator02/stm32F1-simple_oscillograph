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


//static KEY_TypeDef Key[2] = {{0,0,0},{0,0,0}};

static uint8_t Key1Logic = 0;
static uint8_t Key1ONCounts = 0;
static uint8_t Key1OFFCounts = 0;

static uint8_t Key2Logic = 0;
static uint8_t Key2ONCounts = 0;
static uint8_t Key2OFFCounts = 0;

uint8_t Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin)
{	
	uint8_t* KeyLogic;
	uint8_t* KeyONCounts;
	uint8_t* KeyOFFCounts;

	//检查按下的是哪一个按钮
	switch ((uint32_t)GPIOx)
	{
	case ((uint32_t)KEY1_GPIO_PORT):
		switch (GPIO_Pin)
		{
		case KEY1_GPIO_PIN:
			KeyLogic = &Key1Logic;
			KeyONCounts = &Key1ONCounts;
			KeyOFFCounts = &Key1OFFCounts;
			break;
		
		//port和pin不匹配
		default:
			printf("error: GPIO port pin not match\r\n");
			return KEY_IDLE;
			
		}
		break;

	case ((uint32_t)KEY2_GPIO_PORT):
		switch (GPIO_Pin)
		{
		case KEY2_GPIO_PIN:
			KeyLogic = &Key2Logic;
			KeyONCounts = &Key2ONCounts;
			KeyOFFCounts = &Key2OFFCounts;
			break;

		//port和pin不匹配
		default:
			printf("error: GPIO port pin not match\r\n");
			return KEY_IDLE;
			
		}
		break;

	default:
		printf("error: key do not exist\r\n");
		return KEY_IDLE;
	}


	/* 检测按下、松开、长按 */
	switch (*KeyLogic)
	{
	case KEY_ON:
		switch (GPIO_ReadInputDataBit(GPIOx,GPIO_Pin))
		{
		case KEY_ON:
			(*KeyOFFCounts) = 0;
			(*KeyONCounts)++;
			if(*KeyONCounts >= HOLD_COUNTS)
			{
				*KeyONCounts = 0;
				*KeyLogic = KEY_HOLD;
				return KEY_HOLD;
			}
			return KEY_IDLE;

		case KEY_OFF:
			(*KeyOFFCounts)++;
			if(*KeyOFFCounts >= SHAKES_COUNTS)
			{
				*KeyLogic = KEY_OFF;
				*KeyONCounts = 0;
				*KeyOFFCounts = 0;
				return 	KEY_OFF;	
			}
			return KEY_IDLE;

		default:
			break;
		}



	case KEY_OFF:
		switch (GPIO_ReadInputDataBit(GPIOx,GPIO_Pin))
		{
		case KEY_ON:
			(*KeyONCounts)++;
			if(*KeyONCounts >= SHAKES_COUNTS)
			{
				*KeyLogic = KEY_ON;
				*KeyONCounts = 0;
				*KeyOFFCounts = 0;
				return 	KEY_ON;	
			}
			return KEY_IDLE; 

		case KEY_OFF:
			(*KeyONCounts) = 0;
			return KEY_IDLE; 
		default:
			break;
		}



	case KEY_HOLD:
		switch (GPIO_ReadInputDataBit(GPIOx,GPIO_Pin))
		{
		case KEY_ON:
			return KEY_HOLD;

		case KEY_OFF:
			(*KeyOFFCounts)++;
			if(*KeyOFFCounts >= SHAKES_COUNTS)
			{
				*KeyLogic = KEY_OFF;
				*KeyONCounts = 0;
				*KeyOFFCounts = 0;
				return 	KEY_OFF;	
			}
			return KEY_IDLE;

		default:
			break;
		}


	default:
		break;
	}

	return 10;
		
}
/*********************************************END OF FILE**********************/
