#include<stc15f2k60s2.h>
typedef unsigned char uchar;
typedef unsigned int uint;
sbit S7=P3^0;
uchar Key(char *p)
{
	uchar Key_Status=0;
	uchar Key_Value=*p;
	if(Key_Status==0&&Key_Value==0)
	{
		TR1=1;
		Key_Status=1;
	}
	if(Key_Status==1&&TF1==1)
	{
		TR1=0;
	}
	else
	{
		return 0;
	}
}

void Timer1Init(void)		//10����@11.0592MHz
{
	AUXR &= 0xBF;		//��ʱ��ʱ��12Tģʽ
	TMOD &= 0x0F;		//���ö�ʱ��ģʽ
	TL1 = 0x00;		//���ö�ʱ��ֵ
	TH1 = 0xDC;		//���ö�ʱ��ֵ
	TF1 = 0;		//���TF1��־
	EA=1;
	ET1=1;
}


int main()
{
	Timer1Init(void);
	P2=(P2&0X1F)|0X40;
	P0=0X00;
	P2=(P2&0X1F)|0X40;
	while(1)
	{
		if(Key(S7))
		{
			P0=0X00;
		}
	}
}