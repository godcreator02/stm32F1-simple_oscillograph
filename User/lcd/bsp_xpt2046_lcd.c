/**
  ******************************************************************************
  * @file    bsp_ili9341_lcd.c
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   触摸屏驱动
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 F103-霸道 STM32 开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 

#include "./lcd/bsp_xpt2046_lcd.h"
#include "./lcd/bsp_ili9341_lcd.h"
#include "./flash/bsp_spi_flash.h"
#include <stdio.h> 
#include <string.h>

/******************************* 声明 XPT2046 相关的静态函数 ***************************/
static void                   XPT2046_DelayUS                       ( __IO uint32_t ulCount );
static void                   XPT2046_WriteCMD                      ( uint8_t ucCmd );
static uint16_t               XPT2046_ReadCMD                       ( void );

/**
  * @brief  XPT2046 初始化函数
  * @param  无
  * @retval 无
  */	
void XPT2046_Init ( void )
{

  GPIO_InitTypeDef  GPIO_InitStructure = {0};
	

  /* 开启GPIO时钟 */
  RCC_APB2PeriphClockCmd ( XPT2046_SPI_GPIO_CLK|XPT2046_PENIRQ_GPIO_CLK, ENABLE );

  /* 模拟SPI GPIO初始化 */          
  GPIO_InitStructure.GPIO_Pin=XPT2046_SPI_CLK_PIN;
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_10MHz ;	  
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
  GPIO_Init(XPT2046_SPI_CLK_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = XPT2046_SPI_MOSI_PIN;
  GPIO_Init(XPT2046_SPI_MOSI_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = XPT2046_SPI_MISO_PIN; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;      
  GPIO_Init(XPT2046_SPI_MISO_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = XPT2046_SPI_CS_PIN; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz ;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;      
  GPIO_Init(XPT2046_SPI_CS_PORT, &GPIO_InitStructure); 
   
  /* 拉低片选，选择XPT2046 */
  XPT2046_CS_DISABLE();		
								
	//触摸屏触摸信号指示引脚
  GPIO_InitStructure.GPIO_Pin = XPT2046_PENIRQ_GPIO_PIN;       
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 // 上拉输入
  GPIO_Init(XPT2046_PENIRQ_GPIO_PORT, &GPIO_InitStructure);	
}


/**
  * @brief  用于 XPT2046 的简单微秒级延时函数
  * @param  nCount ：延时计数值，单位为微妙
  * @retval 无
  */	
static void XPT2046_DelayUS ( __IO uint32_t ulCount )
{
	uint32_t i;


	for ( i = 0; i < ulCount; i ++ )
	{
		uint8_t uc = 12;     //设置值为12，大约延1微秒  
	      
		while ( uc -- );     //延1微秒	

	}
	
}

/**
  * @brief  XPT2046 的写入命令
  * @param  ucCmd ：命令
  *   该参数为以下值之一：
  *     @arg 0x90 :通道Y+的选择控制字
  *     @arg 0xd0 :通道X+的选择控制字
  * @retval 无
  */
static void XPT2046_WriteCMD ( uint8_t ucCmd ) 
{
	uint8_t i;


	XPT2046_MOSI_0();
	
	XPT2046_CLK_LOW();

	for ( i = 0; i < 8; i ++ ) 
	{
		( ( ucCmd >> ( 7 - i ) ) & 0x01 ) ? XPT2046_MOSI_1() : XPT2046_MOSI_0();
		
	  XPT2046_DelayUS ( 5 );
		
		XPT2046_CLK_HIGH();

	  XPT2046_DelayUS ( 5 );

		XPT2046_CLK_LOW();
	}
}

/**
  * @brief  XPT2046 的读取命令
  * @param  无
  * @retval 读取到的数据
  */
static uint16_t XPT2046_ReadCMD ( void ) 
{
	uint8_t i;
	uint16_t usBuf=0, usTemp;
	
  
	XPT2046_MOSI_0();

	XPT2046_CLK_HIGH();

	for ( i=0;i<12;i++ ) 
	{
		XPT2046_CLK_LOW();    
	
		usTemp = XPT2046_MISO();
		
		usBuf |= usTemp << ( 11 - i );
	
		XPT2046_CLK_HIGH();
	}
	return usBuf;
}

#define SAMP_CNT 4
#define SAMP_CNT_DIV2 2
/*
*********************************************************************************************************
*	函 数 名: XPT2046_ReadAdc_Fliter
*	功能说明: 选择一个模拟通道，启动ADC，并返回ADC采样结果
*	形    参：_ucCh = 0x90 表示Y通道； 0xd0 表示X通道
*	返 回 值: 滤波后的12位ADC值
*********************************************************************************************************
*/
uint16_t XPT2046_ReadAdc_Fliter(uint8_t channel)
{
  u8 i, j, min;
  u16 temp;
  u16 tempXY[SAMP_CNT];
  
  static uint16_t adc_x = 0,adc_y = 0;

  if(XPT2046_PENIRQ_Read()== XPT2046_PENIRQ_ActiveLevel) 
  {
    for(i = 0;i < SAMP_CNT;i++)
    {
      XPT2046_WriteCMD(channel);
      tempXY[i] = XPT2046_ReadCMD();     
    }
    // 降序排列
    for(i=0; i<SAMP_CNT-1; i++)
    {
      min=i;
      for (j=i+1; j<SAMP_CNT; j++)
      {
        if (tempXY[min] > tempXY[j]) min=j;
      }
      temp = tempXY[i];
      tempXY[i] = tempXY[min];
      tempXY[min] = temp;
    }
   
    // 设定阈值
    if((tempXY[SAMP_CNT_DIV2]-tempXY[SAMP_CNT_DIV2-1]) > 5)
    {
      /* 若两个中间值相差太远，则舍弃这个新数据，返回上一次的触摸数据*/
      if(channel == macXPT2046_CHANNEL_Y )
        return adc_x; //x通道
      else
        return adc_y; //y通道
    
    }
    
   // 求中间值的均值   
   if(channel == macXPT2046_CHANNEL_Y)
   {
     adc_x = (tempXY[SAMP_CNT_DIV2]+tempXY[SAMP_CNT_DIV2-1]) / 2;
      return adc_x;
    }      
    else
    {
      adc_y = (tempXY[SAMP_CNT_DIV2]+tempXY[SAMP_CNT_DIV2-1]) / 2;
      return adc_y;
    }
       
  }
  else
  {
    return 0;   //没有触摸，返回0
  }
}


///**
//	* @brief   检测到触摸中断时调用的处理函数,通过它调用tp_down 和tp_up汇报触摸点
//	*	@note 	 本函数需要在while循环里被调用，也可使用定时器定时调用
//	*			例如，可以每隔5ms调用一次，消抖阈值宏DURIATION_TIME可设置为2，这样每秒最多可以检测100个点。
//	*						可在XPT2046_TouchDown及XPT2046_TouchUp函数中编写自己的触摸应用
//	* @param   none
//	* @retval  none
//	*/
//void XPT2046_TouchEvenHandler(void )
//{
//	  static strType_XPT2046_Coordinate cinfo={-1,-1,-1,-1};
//	
//		if(XPT2046_TouchDetect() == TOUCH_PRESSED)
//		{
//			LED_GREEN;
//			
//			//获取触摸坐标
//			XPT2046_Get_TouchedPoint(&cinfo,strXPT2046_TouchPara);
//			
//			//输出调试信息到串口
//			XPT2046_DEBUG("x=%d,y=%d",cinfo.x,cinfo.y);
//			
//			//调用触摸被按下时的处理函数，可在该函数编写自己的触摸按下处理过程
//			XPT2046_TouchDown(&cinfo);
//			
//			/*更新触摸信息到pre xy*/
//			cinfo.pre_x = cinfo.x; cinfo.pre_y = cinfo.y;  

//		}
//		else
//		{
//			LED_BLUE;
//			
//			//调用触摸被释放时的处理函数，可在该函数编写自己的触摸释放处理过程
//			XPT2046_TouchUp(&cinfo); 
//			
//			/*触笔释放，把 xy 重置为负*/
//			cinfo.x = -1;
//			cinfo.y = -1; 
//			cinfo.pre_x = -1;
//			cinfo.pre_y = -1;
//		}

//}


/***************************end of file*****************************/


