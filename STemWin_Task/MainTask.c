/**
  *********************************************************************
  * @file    MainTask.c
  * @author  fire
  * @version V1.0
  * @date    2019-xx-xx
  * @brief   FreeRTOS v9.0.0 + STM32 工程模版
  *********************************************************************
  * @attention
  *
  * 实验平台:野火  STM32 F103 开发板
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */

/*******************************************************************************
     * 包含的头文件
     ******************************************************************************/

/* STemWIN头文件 */

#include "GUI.h"
#include "DIALOG.h"
#include "./bsp_all.h"
#include "MainTask.h"


void MainTask(void)
{   
    GUI_Init();
    GUI_CURSOR_Show();
    WM_SetCreateFlags(WM_CF_MEMDEV);
    GUI_SetBkColor(GUI_BLUE_98);
    GUI_Clear();

    GUI_DispStringAt("Hello World",10,10);
    while (1)
    {
        GUI_Delay(50);
    }
    
}
