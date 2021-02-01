#ifndef  _MAINTASK_H_
#define  _MAINTASK_H_

#include "DSO.h"


typedef struct {                //  文本框通用结构体
    int Handle;
    const char* stitle;
    char* sinfo;
    WIGET Text;
    int CornerSize;
    int Space;
}TEXTSTRUCT;


typedef struct {                // 波形预览小窗结构体
    int Handle;
    int TriggerPos;
    int StartPos;
    int CoverLength;
    WIGET size;
}GRAPHPREWIN_STRUCT;

typedef struct {                //  示波器显示文本结构体
    char TimeBase[10];
    char VoltageBase[10];
    char XPos[10];
    char YPos[10];
    char TriggerLevel[10];
}DSOALLSTRING;


//  右边的8个文本框的显示用全部东西
#define     RIGHTTEXT_XSIZE             50
#define     RIGHTTEXT_YSIZE             23
#define     RIGHTTEXT_HALF_YSIZE        (int)(RIGHTTEXT_YSIZE / 2)
#define     RIGHTTEXT_CSIZE             4
#define     RIGHTTEXT_SPACE             1
#define     RIGHTTEXT_MAXNUMS           8



// 正下方4个文本框显示用
#define     BOTTOMTEXT_XSIZE             82
#define     BOTTOMTEXT_YSIZE             16
#define     BOTTOMTEXT_SPACE             2
#define     BOTTOMTEXT_MAXNUMS           3

// 上方文本框
#define     UPTEXT_L_XSIZE             28
#define     UPTEXT_R_XSIZE             60
#define     UPTEXT_YSIZE               16
#define     UPTEXT_SPACE               2
#define     UPTEXT_MAXNUMS             3

// 上方波形预览条
#define     GRAPHPRE_XSIZE               127
#define     GRAPHPRE_YSIZE               16
#define     GRAPHPRE_MAXNUMS             1

enum { channel, tbase, vbase, xpos, ypos, trlevel, mode, trmode };
enum { vpp, freq, period, bottomTBD };
enum { status, upTBD, sps };
enum { R, B, U};

I16 GetTextHandle(I8 Position, I8 Index);


#endif  //end _MAINTASK_H_



