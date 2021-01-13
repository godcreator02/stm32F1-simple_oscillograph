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

const uint8_t SPSPscNums = 8;
const uint16_t SPSPscForADC[SPSPscNums] = {5, 10, 20, 50, 100, 200, 500, 1000};
const char *SPSPscForShow[SPSPscNums] = {"200k*6 ", "100k", "50k", "20k", "10k", "5k", "2k", "1k"};

const uint8_t FrqPscNums = 10;
const uint16_t FrqPscForDAC[FrqPscNums] = {1, 2, 5, 10, 20, 50, 100, 200, 500, 1000};
const char *FrqPscForShow[FrqPscNums] = {"112500", "56250", "22500", "11250", "5625", "2250", "1125", "562.5",
                                         "225", "112.5"};

int CirMinus(int x, int y)
{
    if ((x - y) == -1)
    {
        return Wave_BufferTempSize;
    }
    if ((x - y) == -2)
    {
        return (Wave_BufferTempSize - 1);
    }
    return (x - y);
}

void DataToDisplay(void)
{
    int i, j;
    double y1, y2, x0, x1, x2;

    /* 拷贝数据到Wave_BufferTemp，避免DMA过快覆盖数组数据 */
    /* 获取ADC_DMA当前位置 */
    ADC_DMA_Pos = ADC_DataSize - DMA_GetCurrDataCounter(ADC_DMA_CHANNEL);
    //printf("ADC_DMA当前位置1 = %d\n",ADC_DMA_Pos);

    memset(Wave_BufferTemp, 0, Wave_BufferTempSize);

    if (ADC_DMA_Pos < Wave_BufferTempSize)
    {
        /* 用于校准拷贝数组时的位置 */
        Array_DMA_Pos = Wave_BufferTempSize - ADC_DMA_Pos;

        j = 0;
        //printf("pos = %d\n",(ADC_DataSize - Array_DMA_Pos));
        for (i = ADC_DataSize - Array_DMA_Pos; i < ADC_DataSize; i++)
        {

			Wave_BufferTemp[j] = ADC_ConvertedValue[i];

            j++;
        }
        //printf("j1a = %d\n",j);
        for (i = 0; i < ADC_DMA_Pos; i++)
        {

			Wave_BufferTemp[j] = ADC_ConvertedValue[i];
            j++;
        }
        //printf("j1b = %d\n",j);
    }
    else
    {
        j = 0;
        for (i = ADC_DMA_Pos - Wave_BufferTempSize; i < ADC_DMA_Pos; i++)
        {
			Wave_BufferTemp[j] = ADC_ConvertedValue[i];
            j++;
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
        if (Wave_BufferTemp[i] > MaxValue)
        {
            MaxValue = Wave_BufferTemp[i];
        }
        if (Wave_BufferTemp[i] < MinValue)
        {
            MinValue = Wave_BufferTemp[i];
        }
    }
    PPValue = MaxValue - MinValue;

    /* 从Wave_BufferTemp拷贝到显示用的数组 */
    for (i = 0; i < 240; i++)
    {
        ADC_DataShow[i] = -(Wave_BufferTemp[(i + j)] / 23);

    }
}

void DrawGraph(void)
{
    GUI_SetColor(GUI_YELLOW);
    GUI_DrawGraph((signed short *)ADC_DataShow, GUI_COUNTOF(ADC_DataShow), GraphRect.x0, GraphRect.y1);
}
