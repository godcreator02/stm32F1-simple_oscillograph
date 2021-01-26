#include "MainTask.h"
#include "./bsp_all.h"

uint16_t ADC_DataShow[ADC_BufferSize];
uint16_t Wave_BufferTemp[Wave_BufferTempSize];

uint16_t ADC_DMA_Pos;
uint16_t ADC_DMA_Pos_Last = 0;
uint16_t Array_DMA_Pos;

uint16_t AutoTriggerLevel = 2048;
uint16_t MaxValue = 0;
uint16_t MinValue = 4096;
uint16_t PPValue = 0;



int CirMinus(int x, int y, int16_t BufferSize)
{
    if ((x - y) == -1)
    {
        return BufferSize;
    }
    if ((x - y) == -2)
    {
        return (BufferSize - 1);
    }
    return (x - y);
}


uint16_t DataProcess(void)
{
    uint16_t i, j;
    /* 拷贝数据到Wave_BufferTemp，避免DMA过快覆盖数组数据 */
    /* 获取ADC_DMA当前位置 */
    ADC_DMA_Pos = ADC_DataSize - DMA_GetCurrDataCounter(ADC_DMA_CHANNEL);
    //printf("ADC_DMA当前位置1 = %d\n",ADC_DMA_Pos);

    memset(Wave_BufferTemp, 0, Wave_BufferTempSize);

    if (ADC_DMA_Pos < Wave_BufferTempSize)
    {
        /* 用于校准拷贝数组时的位置 */
        Array_DMA_Pos = Wave_BufferTempSize - ADC_DMA_Pos;

        //printf("pos = %d\n",(ADC_DataSize - Array_DMA_Pos));
        for (j = 0, i = ADC_DataSize - Array_DMA_Pos; i < ADC_DataSize; i++, j++)
        {
			Wave_BufferTemp[j] = ADC_ConvertedValue[i]; 
        }
        //printf("j1a = %d\n",j);
        for (i = 0; i < ADC_DMA_Pos; i++, j++)
        {
			Wave_BufferTemp[j] = ADC_ConvertedValue[i];
        }
        //printf("j1b = %d\n",j);
    }
    else
    {
        for (j = 0, i = ADC_DMA_Pos - Wave_BufferTempSize; i < ADC_DMA_Pos; i++, j++)
        {
			Wave_BufferTemp[j] = ADC_ConvertedValue[i];
        }
        //printf("j2 = %d\n",j);
    }

    //ADC_DMA_Pos = ADC_DataSize - DMA_GetCurrDataCounter(ADC_DMA_CHANNEL);
    //printf("ADC_DMA当前位置2 = %d\n\n",ADC_DMA_Pos);

    //printf("\n");

    /* 自动触发，上升沿 */

    j = 0;
    while (j < (Wave_BufferTempSize - ADC_BufferSize))
    {
        if (Wave_BufferTemp[j] < AutoTriggerLevel && Wave_BufferTemp[j + 1] >= AutoTriggerLevel)
        {
            break;
        }
        else
        {
            j++;
        }
    }

    //printf("显示起点 j = %d" , j);

    /* 对Wave_BufferTemp做一些处理----峰峰值 */
    MaxValue = 0;
    MinValue = 4096;
    PPValue = 0;
    for (i = 0; i < Wave_BufferTempSize; i++)
    {
        MaxValue = ( Wave_BufferTemp[i] > MaxValue ) ? Wave_BufferTemp[i] : MaxValue;
        MinValue = ( Wave_BufferTemp[i] < MinValue ) ? Wave_BufferTemp[i] : MinValue;
    }
    PPValue = MaxValue - MinValue;

    return j;
}

/* 从Wave_BufferTemp拷贝到显示用的数组 */
/* 因子都是缩小几倍,正为右、上移，负为左、下移 */
void ShowBufferProcess(uint16_t j, uint8_t xScaleFactor, uint8_t yScaleFactor, int16_t xShift, int16_t yShift)
{

    j = ((Wave_BufferTempSize - j - xShift) < ADC_BufferSize * xScaleFactor) 
        ? Wave_BufferTempSize - ADC_BufferSize * xScaleFactor
        : j + xShift;

    for (uint8_t i = 0; i < ADC_BufferSize; i++)
    {
        ADC_DataShow[i] = -(Wave_BufferTemp[(i * xScaleFactor + j)] / 23 / yScaleFactor -yShift);
    }

}

uint16_t readSomeValue(uint8_t n)
{
    switch (n)
    {
    case 0:
        return MaxValue;
    case 1:
        return MinValue;
    case 2:
        return PPValue;
    default:
        printf("error: readValue wrong");
        return 555;
    }
}
