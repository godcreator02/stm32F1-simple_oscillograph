/******************** (C) COPYRIGHT 2008 STMicroelectronics ********************
* File Name          : stm32_dsp.h
* Author             : MCD Application Team
* Version            : V1.0.1
* Date               : 10/20/2008
* Description        : This source file contains prototypes of DSP functions
********************************************************************************
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
* WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH SOFTWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32F10x_DSP_H
#define __STM32F10x_DSP_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
//#include "stm32f1xx_hal.h"

#define u8 uint8_t
#define u16 uint16_t
#define u32 uint32_t
#define s16 int16_t


/* Exported types ------------------------------------------------------------*/
/* for block FIR module */
typedef struct {
  u16 *h;
  u32 nh;
} COEFS;

/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/* FIR 16-bit filter in assembly */
void fir_16by16_stm32(void *y, void *x, COEFS *c, u32 N);

/* PID controller in C, error computed outside the function */
u16 DoPID(u16 Error, u16 *Coeff);

/* Full PID in C, error computed inside the function */
u16 DoFullPID(u16 In, u16 Ref, u16 *Coeff);

/* PID controller in assembly, error computed outside the function */
u16 PID_stm32(u16 Error, u16 *Coeff);

/* Radix-4 complex FFT for STM32, in assembly  */
/* 64 points*/
void cr4_fft_64_stm32(void *pssOUT, void *pssIN, u16 Nbin);
/* 256 points */
void cr4_fft_256_stm32(void *pssOUT, void *pssIN, u16 Nbin);
/* 1024 points */
void cr4_fft_1024_stm32(void *pssOUT, void *pssIN, u16 Nbin);

/* IIR filter in assembly */
void iirarma_stm32(void *y, void *x, u16 *h2, u16 *h1, u32 ny );

/* IIR filter in C */
void iir_biquad_stm32(u16 *y, u16 *x, s16 *IIRCoeff, u16 ny);

#endif /* __STM32F10x_DSP_H */

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/
