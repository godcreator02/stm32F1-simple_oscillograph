/*********************************************************************
*                    SEGGER Microcontroller GmbH                     *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2018  SEGGER Microcontroller GmbH                *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.48 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only  be used  in accordance  with  a license  and should  not be  re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : TOUCH_Calibrate.c
Purpose     : Demonstrates how a touch screen can be calibrated at run time
Requirements: WindowManager - (X)
              MemoryDevices - ( )
              AntiAliasing  - ( )
              VNC-Server    - ( )
              PNG-Library   - ( )
              TrueTypeFonts - ( )
---------------------------END-OF-HEADER------------------------------
*/

#include "DIALOG.h"

/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define NUM_CALIB_POINTS  5  //校准点数

/*********************************************************************
*
*       Static Data
*
**********************************************************************
*/
static int _aRefX[NUM_CALIB_POINTS];
static int _aRefY[NUM_CALIB_POINTS];
static int _aSamX[NUM_CALIB_POINTS];
static int _aSamY[NUM_CALIB_POINTS];

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       使用这个来读取ADC的值
*/
void TOUCH_First_Calibrate(void) {
  GUI_CURSOR_Show();
  GUI_CURSOR_Select(&GUI_CursorCrossL);
  GUI_SetBkColor(GUI_WHITE);
  GUI_SetColor(GUI_BLACK);
  GUI_Clear();
  GUI_DispStringAt("Measurement of\nA/D converter values\n",0,10);
  while (1) {
    GUI_PID_STATE TouchState;
    int xPhys, yPhys;
    GUI_TOUCH_GetState(&TouchState);  /* Get the touch position in pixel */
    xPhys = GUI_TOUCH_GetxPhys();     /* Get the A/D mesurement result in x */
    yPhys = GUI_TOUCH_GetyPhys();     /* Get the A/D mesurement result in y */
    /* Display the measurement result */
    GUI_SetColor(GUI_BLUE);
    GUI_DispStringAt("Analog input:\n", 10, 30);
    GUI_GotoY(GUI_GetDispPosY() + 2);
    GUI_DispString("x:");
    GUI_DispDec(xPhys, 4);
    GUI_DispString(", y:");
    GUI_DispDec(yPhys, 4);
    /* Display the according position */
    GUI_SetColor(GUI_RED);
    GUI_GotoY(GUI_GetDispPosY() + 4);
    GUI_DispString("\nPosition:\n");
    GUI_GotoY(GUI_GetDispPosY() + 2);
    GUI_DispString("x:");
    GUI_DispDec(TouchState.x,4);
    GUI_DispString(", y:");
    GUI_DispDec(TouchState.y,4);
    /* Wait a while */
    GUI_Delay(100);
  };
}

/*********************************************************************
*
*       _Calibrate
*
* Purpose:
*   Calibrates the touch screen. Please note that this sample assumes
*   that the 'Driver API for analog touch screens' is used.
*/
static void _Calibrate(void) {
  GUI_PID_STATE State;
  int i;
  int xSize, ySize;

  xSize = LCD_GetXSize();
  ySize = LCD_GetYSize();
  
  /* 根据LCD尺寸计算参考点 */
  _aRefX[0] = (xSize * 5) / 100;
  _aRefY[0] = (ySize * 5) / 100;
  _aRefX[1] = xSize - (xSize * 5) / 100;
  _aRefY[1] = _aRefY[0];
  _aRefX[2] = _aRefX[1];
  _aRefY[2] = ySize - (ySize * 5) / 100;
  _aRefX[3] = _aRefX[0];
  _aRefY[3] = _aRefY[2];
  _aRefX[4] = xSize / 2;
  _aRefY[4] = ySize / 2;
  
  /* 在LCD上绘制参考点 */
  GUI_TOUCH_GetState(&State);
  State.Pressed = 0;
  GUI_SetPenSize(3);
  for (i = 0; i < NUM_CALIB_POINTS; i++) {
    GUI_Clear();
    GUI_DispStringHCenterAt("Please touch the point", LCD_GetXSize() / 2, LCD_GetYSize() / 2 - 60);
    GUI_DrawCircle(_aRefX[i], _aRefY[i], 5);
    while (State.Pressed != 1) {
      GUI_Delay(250);
      GUI_TOUCH_GetState(&State);
    }
    if (State.Pressed == 1) {
      /* 储存采样点 */
      _aSamX[i] = GUI_TOUCH_GetxPhys();
      _aSamY[i] = GUI_TOUCH_GetyPhys();
    }
    State.Pressed = 0;
    GUI_Delay(250);
  }
  
  /* 将测量点传递给emWin */
  GUI_TOUCH_CalcCoefficients(NUM_CALIB_POINTS, _aRefX, _aRefY, _aSamX, _aSamY, xSize, ySize);
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       MainTask
*/
void TOUCH_Runtime_Calibrate(void) {
  GUI_PID_STATE State;
  //
  // Calibrate touch ...
  //
  _Calibrate();
  //
  // ... and play with it
  //
  GUI_CURSOR_Show();
  GUI_Clear();
	while(1) 
	{
		GUI_TOUCH_GetState(&State);
		if (State.Pressed == 1) 
		{
		  GUI_FillCircle(State.x, State.y, 3);
		}
		GUI_Delay(10);
	}
}

/*************************** End of file ****************************/
