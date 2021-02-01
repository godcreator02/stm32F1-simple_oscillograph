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
	RCC_APB2PeriphClockCmd(KEY1_GPIO_CLK | KEY2_GPIO_CLK, ENABLE);

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
static const uint8_t MulClickState[] = 
{
	KEY_1ClICK, KEY_2ClICK, KEY_3ClICK,
};

static KEY_TypeDef Key[KEY_NUMS] =
	{{KEY_OFF, KEY_OFF, 0, 0, 0, KEY_1ClICK},
	 {KEY_OFF, KEY_OFF, 0, 0, 0, KEY_1ClICK}};

uint8_t Key_Scan(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	KEY_TypeDef *KeyTemp;

	//检查按下的是哪一个按钮
	switch ((uint32_t)GPIOx)
	{
	case ((uint32_t)KEY1_GPIO_PORT):
		switch (GPIO_Pin)
		{
		case KEY1_GPIO_PIN:
			KeyTemp = &Key[0];
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
			KeyTemp = &Key[1];
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

	KeyTemp->KeyPhysic = GPIO_ReadInputDataBit(GPIOx, GPIO_Pin);



	/* 检测按下、松开、长按 */
	switch (KeyTemp->KeyLogic)
	{
	case KEY_ON:
		switch (KeyTemp->KeyPhysic)
		{
		//（1，1）中将关闭计数清零，并对开启计数累加直到切换至逻辑长按状态
		case KEY_ON:
			KeyTemp->KeyOFFCounts = 0;
			KeyTemp->KeyONCounts++;
			if (KeyTemp->KeyONCounts >= HOLD_COUNTS)
			{
				KeyTemp->KeyONCounts = 0;
				KeyTemp->KeyLogic = KEY_HOLD;
				return KEY_HOLD;
			}


			return KEY_IDLE;

		//（1，0）中对关闭计数累加直到切换至逻辑关闭状态
		case KEY_OFF:
			KeyTemp->KeyOFFCounts++;
			
			if (KeyTemp->KeyOFFCounts >= SHAKES_COUNTS)
			{
				KeyTemp->KeyLogic = KEY_OFF;
				KeyTemp->KeyOFFCounts = 0;
				
				return KEY_OFF;
			}
			return KEY_IDLE;

		default:
			break;
		}

	case KEY_OFF:
		switch (KeyTemp->KeyPhysic)
		{
		//（0，1）中对开启计数累加直到切换至逻辑开启状态
		case KEY_ON:
			(KeyTemp->KeyONCounts)++;
			if (KeyTemp->KeyONCounts >= SHAKES_COUNTS)
			{
				//KeyTemp->KeyLogic = KEY_ON;
				KeyTemp->KeyLogic = KEY_ON;			
				KeyTemp->KeyONCounts = 0;

				if(KeyTemp->MulClickFlag == KEY_1ClICK)
				{
					KeyTemp->MulClickFlag = KEY_2ClICK;  	//预备双击状态
					return KEY_ON;
				}
				else
				{
					if(KeyTemp->MulClickFlag != KEY_MAX_MULCLICK)
					{
						KeyTemp->MulClickFlag++;
						KeyTemp->MulClickCounts = 0;
						return (KeyTemp->MulClickFlag - 1);
					}
					else
					{
						KeyTemp->MulClickFlag = KEY_1ClICK;
						return KEY_MAX_MULCLICK;
					}	
				}				
			}
			return KEY_IDLE;

		//（0，0）中将开启计数清零，对多击计数
		case KEY_OFF:
			(KeyTemp->KeyONCounts) = 0;
			if(KeyTemp->MulClickFlag != KEY_1ClICK)
			{
				if(KeyTemp->MulClickCounts++ > MULTIPLE_CLICK_COUNTS) 	//超过多击最大间隔时间，关闭多击状态
				{
					KeyTemp->MulClickCounts = 0;
					KeyTemp->MulClickFlag = KEY_1ClICK;				
				}
			}
			
			return KEY_IDLE;
		default:
			break;
		}

	case KEY_HOLD:
		switch (KeyTemp->KeyPhysic)
		{
		//（2，1）对关闭计数清零
		case KEY_ON:
			KeyTemp->KeyOFFCounts = 0;
			return KEY_HOLD;
		//（2，0）对关闭计数累加直到切换至逻辑关闭状态
		case KEY_OFF:
			(KeyTemp->KeyOFFCounts)++;
			if (KeyTemp->KeyOFFCounts >= SHAKES_COUNTS)
			{
				KeyTemp->KeyLogic = KEY_OFF;
				KeyTemp->KeyOFFCounts = 0;
				return KEY_OFF;
			}
			return KEY_IDLE;

		default:
			break;
		}

	
	default:
		break;
	}
	return KEY_ERROR;
}
/*********************************************END OF FILE**********************/
