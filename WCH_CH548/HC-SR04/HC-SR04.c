#include "HC-SR04.h"
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"

sbit TRIG = P4^5  ; //����������
sbit ECHO = P4^4 ;  //����������

UINT16 distance_time = 0;
UINT16 distance = 0;

void SCAN_DIS() //��������ຯ��
{
	 distance_time=0;
	 TRIG=1;
	 mDelayuS(20);
	 TRIG=0;	
	while(!ECHO);
	TR1=1;			    //��������
	while(ECHO);
	TR1=0;				//�رռ���
	distance_time = TH1*256+TL1;
	TH1=0;
	TL1=0;
	distance = distance_time/10;
}







