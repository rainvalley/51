#ifndef __ADC_H__
#define __ADC_H__
#include "CH549.H"
#include "DEBUG.H"
#define  EN_ADC_INT                     0                                                 //ADC中断使能开关
//通道号选择,分别对应P1.0~P1.7, P0.0~P0.7
#define  CH0                            0x0001
#define  CH1                            0x0002
#define  CH2                            0x0004
#define  CH3                            0x0008
#define  CH4                            0x0010
#define  CH5                            0x0020
#define  CH6                            0x0040
#define  CH7                            0x0080
#define  CH8                            0x0100
#define  CH9                            0x0200
#define  CH10                           0x0400
#define  CH11                           0x0800
#define  CH12                           0x1000
#define  CH13                           0x2000
#define  CH14                           0x4000
#define  CH15                           0x8000
#define  CHALL                          0xFFFF
//快速调用宏定义
#define  ADC_ExChannelSelect( ch )      ADC_CHAN = ADC_CHAN & ~MASK_ADC_CHAN | ch         //外部通道取值 0~15
#define  ADC_StartSample( )             ADC_CTRL = bADC_START                             //启动一次ADC采样
//ADC相关子函数
extern void ADC_ExInit( UINT8 AdcClk );
extern void ADC_ChSelect( UINT8 ch );
extern void ADC_InTSInit(void);
#endif
