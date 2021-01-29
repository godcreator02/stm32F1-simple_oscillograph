/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2013  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.22 - Graphical user interface for embedded applications **
emWin is protected by international copyright laws.   Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with a license and should not be re-
distributed in any way. We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : Dialog_All.c
Purpose     : Example demonstrating DIALOG and widgets
----------------------------------------------------------------------
*/

#include <stdlib.h>
#include <stddef.h>
#include "GUI.h"
#include "DIALOG.h"
#include "DROPDOWN.h"


/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
/*********************************************************************
*
*       Dialog resource
*
* This table contAins the info required to create the dialog.
* It has been created manually, but could also be created by a GUI-builder.
*/
#define GUI_ID_FrameGraph           (GUI_ID_USER + 0x01)

#define ID_RightText_channel        (GUI_ID_USER + 0x10)
#define ID_RightText_tbase        (GUI_ID_USER + 0x11)

typedef struct { I16 width, height, pointspace; } GRID;
typedef struct { I16 x0, y0, xsize, ysize; } WIGET;

static short GraphWaveBuffer[250];

static GUI_RECT GraphRect = { 10, 20, 260, 220 };

//  文本框通用结构体

typedef struct {
    int Handle;
    const char* stitle;
    char* sinfo;
    WIGET Text;
    int CornerSize;
    int Space;
}TEXTSTRUCT;

//  右边的8个文本框的显示用全部东西

#define     RIGHTTEXT_XSIZE             50
#define     RIGHTTEXT_YSIZE             23
#define     RIGHTTEXT_HALF_YSIZE        (int)(RIGHTTEXT_YSIZE / 2)
#define     RIGHTTEXT_CSIZE             4
#define     RIGHTTEXT_SPACE             1
#define     RIGHTTEXT_MAXNUMS           8

enum { channel, tbase, vbase, xpos, ypos, trlevel, mode, trmode };

const char *_arighttext[] =
{
    "CHANNEL",
    "TIMEBASE",
    "V BASE",
    "X POS",
    "Y POS",
    "TR LEVEL",
    "MODE",
    "TR MODE",
};

char* RightTextInfo[] =
{
    "CHA",
    "100ms",
    "1V",
    "128ms",
    "2.06V",
    "103mv",
    "DC",
    "CHA-UP",
};

static TEXTSTRUCT RightText[RIGHTTEXT_MAXNUMS] = {0};

static GUI_POINT _TextBkBig[] = {

    {0                                      , 0},
    {RIGHTTEXT_XSIZE - RIGHTTEXT_CSIZE - 1  , 0},
    {RIGHTTEXT_XSIZE - 1                    , RIGHTTEXT_CSIZE - 1},
    {RIGHTTEXT_XSIZE - 1                    , RIGHTTEXT_YSIZE - 1},
    {RIGHTTEXT_CSIZE - 1                    , RIGHTTEXT_YSIZE - 1},
    {0                                      , RIGHTTEXT_YSIZE - RIGHTTEXT_CSIZE - 1},

};

static GUI_POINT _TextBkSmall[] = {
    {RIGHTTEXT_SPACE                        , RIGHTTEXT_HALF_YSIZE - 1},
    {RIGHTTEXT_XSIZE - RIGHTTEXT_SPACE - 1  , RIGHTTEXT_HALF_YSIZE - 1},
    {RIGHTTEXT_XSIZE - RIGHTTEXT_SPACE - 1  , RIGHTTEXT_YSIZE - RIGHTTEXT_SPACE - 1},
    {RIGHTTEXT_SPACE + RIGHTTEXT_CSIZE - 1  , RIGHTTEXT_YSIZE - RIGHTTEXT_SPACE - 1},
    {RIGHTTEXT_SPACE                        , RIGHTTEXT_YSIZE - RIGHTTEXT_CSIZE - RIGHTTEXT_SPACE - 1}
};

// 正下方四个文本框显示用

#define     BOTTOMTEXT_XSIZE             82
#define     BOTTOMTEXT_YSIZE             16
#define     BOTTOMTEXT_SPACE             2
#define     BOTTOMTEXT_MAXNUMS           3

enum { vpp, freq, period, TBD };

static TEXTSTRUCT BottomText[BOTTOMTEXT_MAXNUMS] = { 0 };

char* BottomTextInfo[] =
{
    "Vp-p: 1.65V",
    "Freq: 10Hz",
    "Period: 10ms",
};

/*******************************************************************
*
*       static code
*
********************************************************************
*/
/*********************************************************************
*
*       _cbBottomText
*/
static void _cbBottomText(WM_MESSAGE* pMsg) {
    char* sinfo;
    int i, bottom_textx = 0;
    WM_HWIN hWin = pMsg->hWin;
    GUI_RECT RECT;
    WM_GetWindowRectEx(hWin, &RECT);

    switch (pMsg->MsgId) {
    case WM_PAINT:
        for (i = 0; i < BOTTOMTEXT_MAXNUMS; i++)
        {
            if (hWin == BottomText[i].Handle)
            {
                bottom_textx = i;
            }
        }

        GUI_SetFont(&GUI_Font13B_ASCII);
        //GUI_DispDecAt(pMsg->hWin, 0, 0, 5);
        switch (bottom_textx)
        {
        case vpp: case period:
            GUI_SetColor(GUI_YELLOW);
            
            GUI_FillRoundedRect(0, 0, BOTTOMTEXT_XSIZE, BOTTOMTEXT_YSIZE, 2);
            GUI_AA_FillRoundedRectEx(&RECT,2);
            
            GUI_SetBkColor(GUI_YELLOW);
            GUI_SetColor(GUI_BLACK);
            GUI_DispStringAt(BottomText[bottom_textx].sinfo, 5, 0);
            break;

        case freq:
            GUI_SetColor(GUI_GREEN);
            GUI_FillRoundedRect(0, 0, BOTTOMTEXT_XSIZE, BOTTOMTEXT_YSIZE, 2);

            GUI_SetBkColor(GUI_GREEN);
            GUI_SetColor(GUI_BLACK);
            GUI_DispStringAt(BottomText[bottom_textx].sinfo, 5, 0);
            break;

        default:
            break;
        }

        

        break;
    default:
        WM_DefaultProc(pMsg);
    }
}

/*********************************************************************
*
*       _cbRightText
*/
static void _cbRightText(WM_MESSAGE* pMsg) {
    char stitle[10];
    char* sinfo;
    int i, right_textx = 0;
    WM_HWIN hWin = pMsg->hWin;

    switch (pMsg->MsgId) {
    case WM_PAINT:
        for (i = 0; i < 8; i++)
        {
            if (hWin == RightText[i].Handle)
            {
                right_textx = i;
            }
        }

        TEXT_GetText(hWin, stitle, 10);
        //GUI_DispDecAt(pMsg->hWin, 0, 0, 5);
        switch (right_textx)
        {
        case channel: case vbase: case ypos: case mode:
            GUI_SetColor(GUI_YELLOW);
            GUI_FillPolygon(_TextBkBig, GUI_COUNTOF(_TextBkBig), 0, 0);

            GUI_SetColor(GUI_BLACK);
            GUI_FillPolygon(_TextBkSmall, GUI_COUNTOF(_TextBkSmall), 0, 0);

            GUI_SetFont(&GUI_Font8_ASCII);
            GUI_SetBkColor(GUI_YELLOW);
            GUI_SetColor(GUI_BLACK);
            GUI_DispStringHCenterAt(RightText[right_textx].stitle, RightText[right_textx].Text.xsize / 2, 2);
            break;

        case tbase: case xpos: case trlevel: case trmode:
            GUI_SetColor(GUI_GREEN);
            GUI_FillPolygon(_TextBkBig, GUI_COUNTOF(_TextBkBig), 0, 0);

            GUI_SetColor(GUI_BLACK);
            GUI_FillPolygon(_TextBkSmall, GUI_COUNTOF(_TextBkSmall), 0, 0);

            GUI_SetFont(&GUI_Font8_ASCII);
            GUI_SetBkColor(GUI_GREEN);
            GUI_SetColor(GUI_BLACK);
            GUI_DispStringHCenterAt(stitle, RightText[right_textx].Text.xsize / 2, 2);
            break;

        
        default:
            break;
        }

        GUI_SetBkColor(GUI_BLACK);
        GUI_SetColor(GUI_WHITE);
        GUI_DispStringHCenterAt(RightText[right_textx].sinfo, RightText[right_textx].Text.xsize / 2, 2 + RightText[right_textx].Text.ysize / 2);

        break;
    default:
        WM_DefaultProc(pMsg);
    }
}

static void Draw_GraphBk(void)
{
    // 250 * 200

    int x = GraphRect.x0;
    int y = GraphRect.y0;
    int xsize = GraphRect.x1 - GraphRect.x0;
    int ysize = GraphRect.y1 - GraphRect.y0;

    //一大格宽度、一大格高度、一大格小格数
    GRID Grid = { xsize / 10, ysize / 8 , 5 }; //25,25,5

    int i, j, k;

    //GUI_SetBkColor(GUI_GRAY);
    GUI_SetBkColor(GUI_BLACK);
    GUI_ClearRect(x, y, x + xsize, y + ysize);

    GUI_SetColor(0x111111);

    //水平的点
    for (k = 1; k < 8; k++)
    {
        for (j = 0; j < 10; j++)
        {
            for (i = 0; i < 5; i++)
            {
                GUI_DrawPoint(x + j * Grid.width + i * Grid.pointspace, y + k * Grid.height);
            }
        }
    }

    //垂直的点
    for (k = 0; k < 8; k++)
    {
        for (j = 1; j < 10; j++)
        {
            for (i = 0; i < 5; i++)
            {
                GUI_DrawPoint(x + j * Grid.width, y + k * Grid.height + i * Grid.pointspace);
            }
        }
    }

    GUI_SetColor(0x333333);
    GUI_DrawRectEx(&GraphRect);

    GUI_SetColor(0x666666);
    GUI_DrawLine(x, y + ysize / 2, x + xsize, y + ysize / 2);
    GUI_DrawLine(x + xsize / 2, y, x + xsize / 2, y + ysize);

    for (j = 0, k = 4; j < 10; j++)
    {
        for (i = (!j) ? 1 : 0; i < 1; i++)
        {
            GUI_DrawLine(
                x + j * Grid.width + i * Grid.pointspace,
                y + k * Grid.height - 2,
                x + j * Grid.width + i * Grid.pointspace,
                y + k * Grid.height + 2
            );
        }

    }

    for (j = 5, k = 0; k < 8; k++)
    {
        for (i = (!k) ? 1 : 0; i < 1; i++)
        {
            GUI_DrawLine(
                x + j * Grid.width - 2,
                y + k * Grid.height + i * Grid.pointspace,
                x + j * Grid.width + 2,
                y + k * Grid.height + i * Grid.pointspace
            );
        }
    }


}

static void Draw_Graph(void)
{
    GUI_SetColor(GUI_YELLOW);
    GUI_DrawGraph((signed short*)GraphWaveBuffer, GUI_COUNTOF(GraphWaveBuffer), GraphRect.x0, GraphRect.y1);
}

/*********************************************************************
*
*       _cbBkWindow
*/
static void _cbBkWindow(WM_MESSAGE* pMsg) {
    int NCode, Id;
    WM_HWIN hEdit0, hEdit1, hEdit2, hEdit3, hListBox, hDropd0, hDropd1, hGraph, hText;
    WM_HWIN hWin = pMsg->hWin;
    switch (pMsg->MsgId) {

    case WM_PAINT:
        //GUI_SetBkColor(GUI_GRAY);
        GUI_SetBkColor(0x040005);
        GUI_Clear();
        Draw_GraphBk();
        Draw_Graph();
        
        
        hText = WM_GetDialogItem(hWin, ID_RightText_channel);
        GUI_DispDecAt(hText, 0, 0, 5);
        GUI_DispDecAt(ID_RightText_channel, 0, 10, 5);
        break;

    case WM_KEY:
        switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) {
        case GUI_KEY_ESCAPE:
            
            break;
        case GUI_KEY_ENTER:
            
            break;
        }
        break;
    case WM_NOTIFY_PARENT:
        Id = WM_GetId(pMsg->hWinSrc);    /* Id of widget */
        NCode = pMsg->Data.v;               /* Notification code */
        switch (NCode) {
        case WM_NOTIFICATION_RELEASED:    /* React only if released */
            if (Id == GUI_ID_OK) {          /* OK Button */
                
            }
            if (Id == GUI_ID_CANCEL) {      /* Cancel Button */
                
            }
            break;
        }
        break;
    default:
        WM_DefaultProc(pMsg);
    }
}

void CreateAllWigets(void) 
{
    WM_HWIN hWin;
    int i;

    for (i = 0; i < RIGHTTEXT_MAXNUMS; i++) 
    {
        hWin = TEXT_CreateEx(RightText[i].Text.x0, RightText[i].Text.y0, RIGHTTEXT_XSIZE, RIGHTTEXT_YSIZE, WM_HBKWIN, WM_CF_SHOW, 0, 0, _arighttext[i]);
        WM_SetCallback(hWin, _cbRightText);
        RightText[i].Handle = hWin;
    }

    for (i = 0; i < BOTTOMTEXT_MAXNUMS; i++)
    {
        hWin = TEXT_CreateEx(BottomText[i].Text.x0, BottomText[i].Text.y0, BOTTOMTEXT_XSIZE, BOTTOMTEXT_YSIZE, WM_HBKWIN, WM_CF_SHOW, 0, 0, NULL);
        WM_SetCallback(hWin, _cbBottomText);
        BottomText[i].Handle = hWin;
    }
  
    hWin = TEXT_CreateEx(10, 55, 48, 15, WM_HBKWIN, WM_CF_SHOW, 0, 0, "LText");

    hWin = BUTTON_CreateEx(100, 5, 60, 20, WM_HBKWIN, WM_CF_SHOW, GUI_ID_OK, 0);
    BUTTON_SetText(hWin, "OK");

    hWin = BUTTON_CreateEx(100, 30, 60, 20, WM_HBKWIN, WM_CF_SHOW, GUI_ID_CANCEL, 0);
    BUTTON_SetText(hWin, "CANSEL");
}

void MY_Init(void) 
{
    int i;

    //右边八个显示的初始化
    for (i = 0; i < RIGHTTEXT_MAXNUMS; i++) 
    {
        RightText[i].CornerSize = RIGHTTEXT_CSIZE;
        RightText[i].Handle = -1;
        RightText[i].sinfo = RightTextInfo[i];
        RightText[i].stitle = _arighttext[i];
        RightText[i].Space = RIGHTTEXT_SPACE;
        RightText[i].Text.x0 = 265;
        RightText[i].Text.y0 = 21 + (RIGHTTEXT_YSIZE + 2) * i;
        RightText[i].Text.xsize = RIGHTTEXT_XSIZE;
        RightText[i].Text.ysize = RIGHTTEXT_YSIZE;
    }

    //下面四个显示的初始化
    for (i = 0; i < BOTTOMTEXT_MAXNUMS; i++)
    {
        BottomText[i].CornerSize = 0;
        BottomText[i].Handle = -1;
        BottomText[i].sinfo = BottomTextInfo[i];
        BottomText[i].stitle = NULL;
        BottomText[i].Space = BOTTOMTEXT_SPACE;
        BottomText[i].Text.x0 = 10 + (BOTTOMTEXT_XSIZE + 2) * i;
        BottomText[i].Text.y0 = 222 ;
        BottomText[i].Text.xsize = BOTTOMTEXT_XSIZE;
        BottomText[i].Text.ysize = BOTTOMTEXT_YSIZE;
    }

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
void MainTask(void) {

    int i = 0, j = 0;

    //初始化结构体
    MY_Init();

    GUI_Init();
    WM_SetCallback(WM_HBKWIN, &_cbBkWindow);
    //WM_SetCreateFlags(WM_CF_MEMDEV);  /* Use memory devices on all windows to avoid flicker */
 
    TEXT_SetDefaultTextColor(GUI_DARKBLUE);
    TEXT_SetDefaultFont(&GUI_Font13B_1);

    //创建全部的控件
    CreateAllWigets();

    while (1) {
        for (i = 0; i < GUI_COUNTOF(GraphWaveBuffer); i++)
        {
            GraphWaveBuffer[i] = -(i + j) % 200;
        }
        j++;

        WM_InvalidateRect(WM_HBKWIN, &GraphRect);
        GUI_Delay(100);
        
    }
}

/*************************** End of file ****************************/
