#ifndef __MAINTASK_H
#define __MAINTASK_H

#include "GUI.h"
#include "DIALOG.h"
#include <stdlib.h>
#include "stm32f10x.h"

typedef struct
{
    uint8_t xSacleFactor;
    uint8_t ySacleFactor;
    uint16_t xShift;
    uint16_t yShift;
}SHOWPARAMS;



/* 宏定义 */
#define     SPSUpdateMes        WM_USER+0
#define     FreqUpdateMes       WM_USER+1

#define     SPSPscNums      8
#define     FrqPscNums      10

// 数据记录
#define    Wave_BufferTempSize           2048
#define    ADC_BufferSize                240


extern TEXT_Handle hText0;
extern TEXT_Handle hText1;
extern TEXT_Handle hText2;
extern TEXT_Handle hText3;
extern TEXT_Handle hText4;

/* MainTask的全局变量 */
extern uint16_t t0;
extern uint16_t delta_t;        

extern uint16_t AutoTriggerLevel;               //0--0V  4096--3.3V
extern uint16_t MaxValue;
extern uint16_t MinValue;
extern uint16_t PPValue;

extern const uint16_t SPSPscForADC[];
extern const char *SPSPscForShow[];
extern int8_t SRS_Select;

extern const uint16_t FrqPscForDAC[];
extern const char *FrqPscForShow[];
extern int8_t Frq_Select;

/* 其他文件的全局变量 */

extern SHOWPARAMS showParams;

extern uint16_t ADC_DataShow[ADC_BufferSize];
extern uint16_t Wave_BufferTemp[Wave_BufferTempSize];


/* 界面布局用矩形框框 */
extern GUI_RECT GraphFrameWinRect ;
extern GUI_RECT GraphRect ;

/* 函数 */
void MainTask(void);
uint16_t DataProcess(void);
void ShowBufferProcess(uint16_t j, uint8_t xScaleFactor, uint8_t yScaleFactor, int16_t xShift, int16_t yShift);

uint16_t readSomeValue(uint8_t n);


#endif /* __MAINTASK_H */
