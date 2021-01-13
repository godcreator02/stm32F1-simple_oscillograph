#ifndef __SCREEN_SHOT_H
#define __SCREEN_SHOT_H

#include "GUI.h"

/******************* 截屏用到的全局变量 *******************/
extern FATFS   fs;			/* FatFs文件系统对象 */
extern FIL     file;		/* file objects */
extern UINT    bw;     /* File R/W count */
extern FRESULT result; 
extern FILINFO fno;
extern DIR dir;

void FS_Init(void);
void ScreenShot(void);
void _WriteByte2File(U8 Data, void *p);

#endif /* __SCREEN_SHOT_H */
