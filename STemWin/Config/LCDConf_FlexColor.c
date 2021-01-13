/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2017  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.44 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The  software has  been licensed  to STMicroelectronics International
N.V. a Dutch company with a Swiss branch and its headquarters in Plan-
les-Ouates, Geneva, 39 Chemin du Champ des Filles, Switzerland for the
purposes of creating libraries for ARM Cortex-M-based 32-bit microcon_
troller products commercialized by Licensee only, sublicensed and dis_
tributed under the terms and conditions of the End User License Agree_
ment supplied by STMicroelectronics International N.V.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : LCDConf_FlexColor_Template.c
Purpose     : Display controller configuration (single layer)
---------------------------END-OF-HEADER------------------------------
*/

/**
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2018 STMicroelectronics. 
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license SLA0044,
  * the "License"; You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *                      http://www.st.com/SLA0044
  *
  ******************************************************************************
  */

#include "GUI.h"
#include "GUIDRV_FlexColor.h"
#include "./lcd/bsp_ili9341_lcd.h"
/*********************************************************************
*
*       Layer configuration (to be modified)
*
**********************************************************************
*/

//
// Physical display size
//
#define XSIZE_PHYS macILI9341_Default_Max_Width  // To be adapted to x-screen size
#define YSIZE_PHYS macILI9341_Default_Max_Heigth // To be adapted to y-screen size

#define TOUCH_AD_TOP            3670   //YPhys0
#define TOUCH_AD_BOTTOM         223 //YPhys1
#define TOUCH_AD_LEFT           208   //XPhys0
#define TOUCH_AD_RIGHT          3492 //XPhys1

/*********************************************************************
*
*       Configuration checking
*
**********************************************************************
*/
#ifndef VXSIZE_PHYS
#define VXSIZE_PHYS XSIZE_PHYS
#endif
#ifndef VYSIZE_PHYS
#define VYSIZE_PHYS YSIZE_PHYS
#endif
#ifndef XSIZE_PHYS
#error Physical X size of display is not defined!
#endif
#ifndef YSIZE_PHYS
#error Physical Y size of display is not defined!
#endif
#ifndef GUICC_565
#error Color conversion not defined!
#endif
#ifndef GUIDRV_FLEXCOLOR
#error No display driver defined!
#endif

/*********************************************************************
*
*       Local functions
*
**********************************************************************
*/
/********************************************************************
*
*       LcdWriteReg
*
* Function description:
*   Sets display register
*/
static void LcdWriteReg(U16 Data)
{
    // ... TBD by user
    macFSMC_ILI9341_REG = Data; //modify by fire
}

/********************************************************************
*
*       LcdWriteData
*
* Function description:
*   Writes a value to a display register
*/
static void LcdWriteData(U16 Data)
{
    // ... TBD by user
    macFSMC_ILI9341_RAM = Data; //modify by fire
}

/********************************************************************
*
*       LcdWriteDataMultiple
*
* Function description:
*   Writes multiple values to a display register.
*/
static void LcdWriteDataMultiple(U16 *pData, int NumItems)
{
    while (NumItems--)
    {
        // ... TBD by user
        macFSMC_ILI9341_RAM = *pData++; //modify by fire
    }
}

/********************************************************************
*
*       LcdReadDataMultiple
*
* Function description:
*   Reads multiple values from a display register.
*/
static void LcdReadDataMultiple(U16 *pData, int NumItems)
{
    /* 舍弃ili9341读取的第一个数据为无效数据
  (原来没有使用config.numdummyreads参数的时候需要这个语句)*/
    //*pData = ILI9341_RAM;
    while (NumItems--)
    {
        // ... TBD by user
        *pData++ = macFSMC_ILI9341_RAM; //modify by fire
    }
}

/*********************************************************************
*
*       Public functions
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_X_Config
*
* Function description:
*   Called during the initialization process in order to set up the
*   display driver configuration.
*
*/
void LCD_X_Config(void)
{

    GUI_DEVICE *pDevice;
    CONFIG_FLEXCOLOR Config = {0};
    GUI_PORT_API PortAPI = {0};
    //
    // Set display driver and color conversion
    //
    pDevice = GUI_DEVICE_CreateAndLink(GUIDRV_FLEXCOLOR, GUICC_565, 0, 0);
    //
    // Display driver configuration, required for Lin-driver
    //
    LCD_SetSizeEx(0, XSIZE_PHYS, YSIZE_PHYS);
    LCD_SetVSizeEx(0, VXSIZE_PHYS, VYSIZE_PHYS);
    //
    // Orientation
    //
    Config.FirstCOM = 0;                              //modify by fire
    Config.FirstSEG = 0;                              //modify by fire
//    Config.Orientation = GUI_MIRROR_Y | GUI_MIRROR_X; //modify by fire 竖屏
    Config.Orientation = GUI_SWAP_XY | GUI_MIRROR_Y;  //modify by fire 横屏
    Config.NumDummyReads = 2;                         //modify by fire 读取的第二个数据才是真实数据

    GUIDRV_FlexColor_Config(pDevice, &Config);
    //
    // Set controller and operation mode
    //
    PortAPI.pfWrite16_A0 = LcdWriteReg;
    PortAPI.pfWrite16_A1 = LcdWriteData;
    PortAPI.pfWriteM16_A1 = LcdWriteDataMultiple;
    PortAPI.pfReadM16_A1 = LcdReadDataMultiple;
    GUIDRV_FlexColor_SetFunc(pDevice, &PortAPI, GUIDRV_FLEXCOLOR_F66709, GUIDRV_FLEXCOLOR_M16C0B16); //modify by fire GUIDRV_FLEXCOLOR_F66708

    /*设置触摸原点 */
    GUI_TOUCH_SetOrientation((GUI_MIRROR_X * LCD_GetMirrorXEx(0)) |
                             (GUI_MIRROR_Y * LCD_GetMirrorYEx(0)) |
                             (GUI_SWAP_XY * LCD_GetSwapXYEx(0)));

    /*设置触摸校准 */
    GUI_TOUCH_Calibrate(GUI_COORD_X, 0, 320 - 1,  TOUCH_AD_LEFT,TOUCH_AD_RIGHT);
    GUI_TOUCH_Calibrate(GUI_COORD_Y, 0, 240 - 1, TOUCH_AD_TOP, TOUCH_AD_BOTTOM);
}

/*********************************************************************
*
*       LCD_X_DisplayDriver
*
* Function description:
*   This function is called by the display driver for several purposes.
*   To support the according task the routine needs to be adapted to
*   the display controller. Please note that the commands marked with
*   'optional' are not cogently required and should only be adapted if
*   the display controller supports these features.
*
* Parameter:
*   LayerIndex - Index of layer to be configured
*   Cmd        - Please refer to the details in the switch statement below
*   pData      - Pointer to a LCD_X_DATA structure
*
* Return Value:
*   < -1 - Error
*     -1 - Command not handled
*      0 - Ok
*/
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void *pData)
{
    int r;
    (void)LayerIndex;
    (void)pData;

    switch (Cmd)
    {
    case LCD_X_INITCONTROLLER:
    {
        /* Called during the initialization process in order to set
       up the display controller and put it into operation. 
       If the display controller is not initialized by any 
       external routine this needs to be adapted by the customer...
    */
        ILI9341_Init(); //modify by fire

        return 0;
    }
    default:
        r = -1;
    }
    return r;
}

/*************************** End of file ****************************/
