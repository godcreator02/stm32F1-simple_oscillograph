/**
  *********************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   FreeRTOS V9.0.0  + STM32 固件库例程
  *********************************************************************
  * @attention
  *
  * 实验平台:野火 STM32 全系列开发板 
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  **********************************************************************
  */

/*
*************************************************************************
*                             包含的头文件
*************************************************************************
*/
/* FreeRTOS头文件 */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
/* 开发板硬件bsp头文件 */
#include "./bsp_all.h"
/* STemWIN头文件 */
#include "GUI.h"
#include "MainTask.h"

/* FATFS */
#include "ff.h"
#include "diskio.h"
#include "integer.h"

/**************************** 任务句柄 ********************************/
/* 
 * 任务句柄是一个指针，用于指向一个任务，当任务创建好之后，它就具有了一个任务句柄
 * 以后我们要想操作这个任务都需要通过这个任务句柄，如果是自身的任务操作自己，那么
 * 这个句柄可以为NULL。
 */
/* 创建任务句柄 */
static TaskHandle_t AppTaskCreate_Handle = NULL;
/* LED任务句柄 */
static TaskHandle_t DataProcess_Task_Handle = NULL;
/* Touch任务句柄 */
static TaskHandle_t Touch_Task_Handle = NULL;
/* GUI任务句柄 */
static TaskHandle_t GUI_Task_Handle = NULL;
static TaskHandle_t CPU_Task_Handle = NULL;

/********************************** 内核对象句柄 *********************************/
/*
 * 信号量，消息队列，事件标志组，软件定时器这些都属于内核的对象，要想使用这些内核
 * 对象，必须先创建，创建成功之后会返回一个相应的句柄。实际上就是一个指针，后续我
 * 们就可以通过这个句柄操作这些内核对象。
 *
 * 内核对象说白了就是一种全局的数据结构，通过这些数据结构我们可以实现任务间的通信，
 * 任务间的事件同步等各种功能。至于这些功能的实现我们是通过调用这些内核对象的函数
 * 来完成的
 * 
 */

/******************************* 全局变量声明 ************************************/
/*
 * 当我们在写应用程序的时候，可能需要用到一些全局变量。
 */
FATFS fs; /* FatFs文件系统对象 */
FIL file; /* file objects */
UINT bw;  /* File R/W count */
FRESULT result;
FILINFO fno;
DIR dir;

int8_t SRS_Select = 5;
int8_t Frq_Select = 4;

/*
*************************************************************************
*                             函数声明
*************************************************************************
*/
static void AppTaskCreate(void); /* 用于创建任务 */

static void DataProcess_Task(void *parameter); /* DataProcess_Task */
static void GUI_Task(void *parameter);         /* GUI_Task任务实现 */
static void Touch_Task(void *parameter);
static void CPU_Task(void *pvParameters); /* CPU_Task任务实现 */

static void BSP_Init(void); /* 用于初始化板载相关资源 */

/*****************************************************************
  * @brief  主函数
  * @param  无
  * @retval 无
  * @note   第一步：开发板硬件初始化 
            第二步：创建APP应用任务
            第三步：启动FreeRTOS，开始多任务调度
  ****************************************************************/
int main(void)
{
    BaseType_t xReturn = pdPASS; /* 定义一个创建信息返回值，默认为pdPASS */

    /* 开发板硬件初始化 */
    BSP_Init();

    printf("\r\n ********** emwin DEMO *********** \r\n");

    /* 创建AppTaskCreate任务 */
    xReturn = xTaskCreate((TaskFunction_t)AppTaskCreate,          /* 任务入口函数 */
                          (const char *)"AppTaskCreate",          /* 任务名字 */
                          (uint16_t)512,                          /* 任务栈大小 */
                          (void *)NULL,                           /* 任务入口函数参数 */
                          (UBaseType_t)1,                         /* 任务的优先级 */
                          (TaskHandle_t *)&AppTaskCreate_Handle); /* 任务控制块指针 */
    /* 启动任务调度 */
    if (pdPASS == xReturn)
        vTaskStartScheduler(); /* 启动任务，开启调度 */
    else
        return -1;

    while (1)
        ; /* 正常不会执行到这里 */
}

/***********************************************************************
  * @ 函数名  ： AppTaskCreate
  * @ 功能说明： 为了方便管理，所有的任务创建函数都放在这个函数里面
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  **********************************************************************/
static void AppTaskCreate(void)
{
    BaseType_t xReturn = pdPASS; /* 定义一个创建信息返回值，默认为pdPASS */

    taskENTER_CRITICAL(); //进入临界区

    xReturn = xTaskCreate((TaskFunction_t)DataProcess_Task,        /* 任务入口函数 */
                          (const char *)"DataProcess_Task",        /* 任务名称 */
                          (uint16_t)256,                           /* 任务栈大小 */
                          (void *)NULL,                            /* 任务入口函数参数 */
                          (UBaseType_t)10,                         /* 任务的优先级 */
                          (TaskHandle_t)&DataProcess_Task_Handle); /* 任务控制块指针 */
    if (pdPASS == xReturn)
        printf("创建DataProcess_Task任务成功！\r\n");

    xReturn = xTaskCreate((TaskFunction_t)Touch_Task,        /* 任务入口函数 */
                          (const char *)"Touch_Task",        /* 任务名称 */
                          (uint16_t)256,                     /* 任务栈大小 */
                          (void *)NULL,                      /* 任务入口函数参数 */
                          (UBaseType_t)4,                    /* 任务的优先级 */
                          (TaskHandle_t)&Touch_Task_Handle); /* 任务控制块指针 */
    if (pdPASS == xReturn)
        printf("创建Touch_Task任务成功！\r\n");

    xReturn = xTaskCreate((TaskFunction_t)GUI_Task,        /* 任务入口函数 */
                          (const char *)"GUI_Task",        /* 任务名称 */
                          (uint16_t)2048,                  /* 任务栈大小 */
                          (void *)NULL,                    /* 任务入口函数参数 */
                          (UBaseType_t)3,                  /* 任务的优先级 */
                          (TaskHandle_t)&GUI_Task_Handle); /* 任务控制块指针 */
    if (pdPASS == xReturn)
        printf("创建GUI_Task任务成功！\r\n");

    xReturn = xTaskCreate((TaskFunction_t)CPU_Task,          /* 任务入口函数 */
                          (const char *)"CPU_Task",          /* 任务名字 */
                          (uint16_t)256,                     /* 任务栈大小 */
                          (void *)NULL,                      /* 任务入口函数参数 */
                          (UBaseType_t)5,                    /* 任务的优先级 */
                          (TaskHandle_t *)&CPU_Task_Handle); /* 任务控制块指针 */
    if (pdPASS == xReturn)
        printf("创建CPU_Task任务成功!\r\n");

    vTaskDelete(AppTaskCreate_Handle); //删除AppTaskCreate任务

    taskEXIT_CRITICAL(); //退出临界区
}

/**
  * @brief LED任务主体
  * @note 无
  * @param 无
  * @retval 无
  */


static void DataProcess_Task(void *parameter)
{

    while (1)
    {
        /* 采样率 */
        if (Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)
        {
            printf("Key1ON\n");
        }

        if (Key_Scan(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == KEY_ON)
        {
            printf("Key2ON\n");
        }

        vTaskDelay(20);
    }
}

/**
  * @brief 触摸检测任务主体
  * @note 无
  * @param 无
  * @retval 无
  */
static void Touch_Task(void *parameter)
{
    while (1)
    {
        GUI_TOUCH_Exec(); //触摸屏定时扫描
        vTaskDelay(10);
    }
}

/**
  * @brief GUI任务主体
  * @note 无
  * @param 无
  * @retval 无
  */
static void GUI_Task(void *parameter)
{
    /* 初始化STemWin */

    while (1)
    {
        MainTask();
    }
}

static void CPU_Task(void *parameter)
{
    uint8_t CPU_RunInfo[400]; //保存任务运行时间信息

    while (1)
    {
        memset(CPU_RunInfo, 0, 400); //信息缓冲区清零

        vTaskList((char *)&CPU_RunInfo); //获取任务运行时间信息

        printf("---------------------------------------------\r\n");
        printf("任务名      任务状态 优先级   剩余栈 任务序号\r\n");
        printf("%s", CPU_RunInfo);
        printf("---------------------------------------------\r\n");

        memset(CPU_RunInfo, 0, 400); //信息缓冲区清零

        vTaskGetRunTimeStats((char *)&CPU_RunInfo);

        printf("任务名       运行计数         利用率\r\n");
        printf("%s", CPU_RunInfo);
        printf("---------------------------------------------\r\n\n");
        vTaskDelay(20000); /* 延时500个tick */
    }
}

/**
  * @brief 板级外设初始化
  * @note 所有板子上的初始化均可放在这个函数里面
  * @param 无
  * @retval 无
  */
static void BSP_Init(void)
{
    /* CRC和emWin没有关系，只是他们为了库的保护而做的
   * 这样STemWin的库只能用在ST的芯片上面，别的芯片是无法使用的。
   */
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);

    /*
	 * STM32中断优先级分组为4，即4bit都用来表示抢占优先级，范围为：0~15
	 * 优先级分组只需要分组一次即可，以后如果有其他的任务需要用到中断，
	 * 都统一用这个优先级分组，千万不要再分组，切忌。
	 */
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);

    DAC_Mode_Init();

    ADCx_Init();
    /* LED 初始化 */
    LED_GPIO_Config();

    /* 蜂鸣器初始化 */
    Beep_Init();

    /* 串口初始化	*/
    USART_Config();

    /* 按键初始化	*/
    Key_GPIO_Config();

    /* 触摸屏初始化 */
    XPT2046_Init();

    BASIC_TIM_Init();
}

/********************************END OF FILE****************************/
