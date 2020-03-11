#ifndef __TOUCHKEY_H__
#define __TOUCHKEY_H__
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
#define   EN_ADC_INT                    1
#define   DOWM_THRESHOLD_VALUE          1000                      //按下阈值定义
#define   UP_THRESHOLD_VALUE            50                        //抬起阈值
extern void TouchKey_Init( void );
extern UINT16 TouchKeySelect( UINT8 ch,UINT8 cpw );
#endif
