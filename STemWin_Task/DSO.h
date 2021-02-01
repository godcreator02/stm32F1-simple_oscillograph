#ifndef  _DSO_H_
#define  _DSO_H_

#ifndef   U8
#define U8  unsigned char
#endif
#ifndef   U16
#define U16 unsigned short
#endif
#ifndef   U32
#define U32 unsigned long
#endif
#ifndef   I8
#define I8  signed char
#endif
#ifndef   I16
#define I16 signed short
#endif
#ifndef   I32
#define I32 signed long
#endif

#ifndef   GUI_COUNTOF(a)
    #define GUI_COUNTOF(a)          (sizeof(a) / sizeof(a[0]))
#endif

typedef struct { I16 width, height, pointspace; } GRID;
typedef struct { I16 x0, y0, xsize, ysize; } WIGET;

#define     NUMS_PER_mV               ((double)4096 / (double)3300)

#define     SAMPLE_DEPTH              2000          //  存储深度1000 * 2字节（short）
#define     WAVE_BUFF_SIZE            (SAMPLE_DEPTH / 2)    
#define     SHOW_BUFF_SIZE            250           //  一个屏幕显示250个点
#define     MID_POS_WAVEBUF           (WAVE_BUFF_SIZE / 2)  
#define     MID_POS_SHOWBUF           (SHOW_BUFF_SIZE / 2)  

#define     POINTS_ONE_MOVE           (5l)          //  每次左右移一小格是 5 个点

#define     VOLTAGEMAXGRADE            8           // 电压挡位 0-11,      以1mV为单位1
#define     VOLTAGE_DEFAULT_GRADE      8

#define     SPSMAXGRADE                10           // 采样率挡位 0-10,    以5us为单位1
#define     SPS_DEFAULT_GRADE          5


#define     LEFT_LIMIT                MID_POS_SHOWBUF 
#define     RIGHT_LIMIT               (WAVE_BUFF_SIZE - MID_POS_SHOWBUF)

#define     START_FROM_TRIGGER                  0
#define     INCREASE_XPOS                       1
#define     TRIGGER_AS_END                      2
#define     DECREASE_XPOS                       3
#define     ENOUGH                              10

typedef struct {                //  采样率和时基对应
    I32 SPS;
    I32 TIMEBASE;
}DSO_MATCH_2PARAMS;


typedef struct {                //  示波器物理参数
    //I32 SampleRate;
    const I16 Depth;
    char TriggerMode;
    I16 TriggerLevel;
    I32 XPos;
    I32 YPos;
    I8 TimeBaseGrade;
    I8 VoltageBaseGrade;
}DSOPARAMS;


typedef struct {                //  示波器显示参数
    I16 TriggerPos;
    double XBufPos;
    I16 ShowStartPos;

    double YRatio;
    I16 YStartPos;
}DSOSHOWPARAMS;

typedef struct {                //  波形参数
    I16 MaxValue;
    I16 MinValue;
    I16 PPValue;

    I32 Freq;
    I32 Period;

}WAVEPARAMS;

// 外部申明
extern short WaveBuffer[WAVE_BUFF_SIZE];
extern short GraphShowBuffer[SHOW_BUFF_SIZE];

extern DSOPARAMS DSOParams;
extern DSOSHOWPARAMS DSOShowParams;
extern WAVEPARAMS WaveParams;

extern const DSO_MATCH_2PARAMS _tgrade[SPSMAXGRADE + 1];
extern const I16 _vgrade[VOLTAGEMAXGRADE + 1];


void CalShowStartPos(void);
void CopyDataToWaveBuff(void);

#endif  //end _DSO_H_

