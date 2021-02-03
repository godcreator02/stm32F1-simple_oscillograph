/*	 ADC采样数据处理，有三个数组，ADC原始数据数组，WaveBuff示波器缓冲数组，ShowBuff显示数组
*
*
*/
#include <math.h>
#include <stdio.h>
#include "DSO.h"
#include "stm32f10x.h"
#include "./bsp_adc.h"
#include "./bsp_usart.h"

#include "../DSP/stm32_dsp.h"
#include "../DSP/table_fft.h"

long FFTBuffer[WAVE_BUFF_SIZE / 2]; // 示波器2000的采样深度
long MagArray[WAVE_BUFF_SIZE / 2];  //fft幅值

WAVEPARAMS WaveParams =
    {
        0, 4095, 0, 0, 0};

static uint16_t ADC_DMA_Pos;
static uint16_t DMA_Pos_In_Array;

/* 返回最大频率处  */
void FFT_GetFreq(I32 SPS)
{
    s16 X1, Y1;
    u32 i;
    u32 maxtemp;
    uint16_t pos;

    uint16_t N = 1024;

    cr4_fft_1024_stm32(FFTBuffer, WaveBuffer, N);
    for (i = 0; i < N / 2; i++)
    {
        X1 = (FFTBuffer[i] << 16) >> 16;
        Y1 = (FFTBuffer[i] >> 16);
        {
            float X = N * ((float)X1) / 32768;
            float Y = N * ((float)Y1) / 32768;
            float Mag = sqrt(X * X + Y * Y) / N;
            MagArray[i] = (u32)(Mag * 65536);
        }
    }
    MagArray[0] /= 2; 
    for (i = 2, maxtemp = MagArray[1], pos = 1; i < N / 2; i++)
    {
        maxtemp = (maxtemp > MagArray[i]) ? maxtemp : MagArray[i];
        pos = (maxtemp > MagArray[i]) ? pos : i;
    }

    WaveParams.Freq = (double)SPS / N * pos;

}

void CopyDataToWaveBuff(void)
{
    uint16_t i, j;

    /* 拷贝数据到WaveBuffer，同时避免DMA过快覆盖数组数据 */
    /* 获取ADC_DMA当前位置 */
    ADC_DMA_Pos = ADC_DataSize - DMA_GetCurrDataCounter(ADC_DMA_CHANNEL);

    //printf("ADC_DMA当前位置1 = %d\n",ADC_DMA_Pos);

    if (ADC_DMA_Pos < WAVE_BUFF_SIZE)
    {
        /* 用于校准拷贝数组时的位置 */
        DMA_Pos_In_Array = WAVE_BUFF_SIZE - ADC_DMA_Pos;

        //printf("pos = %d\n",(ADC_DataSize - Array_DMA_Pos));
        for (j = 0, i = ADC_DataSize - DMA_Pos_In_Array; i < ADC_DataSize; i++, j++)
        {
            WaveBuffer[j] = ADC_ConvertedValue[i];
        }
        for (i = 0; i < ADC_DMA_Pos; i++, j++)
        {
            WaveBuffer[j] = ADC_ConvertedValue[i];
        }
    }
    else
    {
        for (j = 0, i = ADC_DMA_Pos - WAVE_BUFF_SIZE; i < ADC_DMA_Pos; i++, j++)
        {
            WaveBuffer[j] = ADC_ConvertedValue[i];
        }
    }


    /* 对WaveBuffer做一些处理----峰峰值 */
    WaveParams.MaxValue = 0;
    WaveParams.MinValue = 4096;
    WaveParams.PPValue = 0;
    for (i = 0; i < WAVE_BUFF_SIZE; i++)
    {
        WaveParams.MaxValue = (WaveBuffer[i] > WaveParams.MaxValue) ? WaveBuffer[i] : WaveParams.MaxValue;
        WaveParams.MinValue = (WaveBuffer[i] < WaveParams.MinValue) ? WaveBuffer[i] : WaveParams.MinValue;
    }
    WaveParams.PPValue = WaveParams.MaxValue - WaveParams.MinValue;

    
}
