#ifndef __Infrared_H_
#define	__Infrared_H_
            
#include ".\Public\CH549.H"
sbit Pin_Decode = P3^3;
void Int1Init();
extern UINT8 IF_DC_KEY;//����ֵ1--17
extern UINT8 INF_CNT_f;//��־λ1--�������0--���ڴ���

#endif