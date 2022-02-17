#ifndef _CONFIG_H
#define	_CONFIG_H

#define BS_FILE "bs_check.dat"

//#define         PSNR             1

//#define	RATE_CONTROL_ON		 1
#define			DB_OPEN  		 1
#define			SAO_OPEN		 1

//#define  DUMP_CUR_MB_SIM
//#define			DUMP_BS_CHECK
//#define			DUMP_REC_DEBUG

/**************************************************************************************
*                                Encoder Setting                                      *
***************************************************************************************/  
#define f_416x240 1
#define p1080 0

//FRAMEWIDTH & FRAMEHEIGHT
#ifdef QCIF
#define FRAMEWIDTH 176
#define FRAMEHEIGHT 144
#elif CIF
#define FRAMEWIDTH 352
#define FRAMEHEIGHT 288
#elif D1
#define FRAMEWIDTH 720
#define FRAMEHEIGHT 576
#elif p720
#define FRAMEWIDTH 1280
#define FRAMEHEIGHT 720
#elif p1080
#define FRAMEWIDTH 1920
#define FRAMEHEIGHT 1080
#elif f_416x240
#define FRAMEWIDTH 416
#define FRAMEHEIGHT 240
#elif f_832x480
#define FRAMEWIDTH 832
#define FRAMEHEIGHT 480
#elif f_1024x768
#define FRAMEWIDTH 1024
#define FRAMEHEIGHT 768
#elif f_2560x1600
#define FRAMEWIDTH 2560
#define FRAMEHEIGHT 1600
#endif

#define MB_X_TOTAL				( (FRAMEWIDTH +(f_LCU_SIZE-1)) / f_LCU_SIZE )
#define MB_Y_TOTAL				( (FRAMEHEIGHT+(f_LCU_SIZE-1)) / f_LCU_SIZE )

#define FRAME_PER_SECOND		50
#define GOP_LENGTH				5
#define FRAME_TOTAL				10

#define f_LCU_SIZE				64

#define LOG2_LCU_SIZE			6

#define f_INTER_DEBUG           0

#define SEARCH_RANGE			32
#define SW_W                    (f_LCU_SIZE+SEARCH_RANGE)
#define SW_H                    (f_LCU_SIZE+SEARCH_RANGE)

#define BIT_DEPTH				8
#define PIXEL_BYTE              ( (BIT_DEPTH+7) / 8 )

#define BS_BUF_SIZE				20000

#define INIT_QP					22

#define f264_SLICE_MAX			4
#define f264_NAL_MAX			(4 + f264_SLICE_MAX)

#endif	/* _CONFIG_H */