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

void Timer1Init(void)		//10毫秒@11.0592MHz
{
	AUXR &= 0xBF;		//定时器时钟12T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0x00;		//设置定时初值
	TH1 = 0xDC;		//设置定时初值
	TF1 = 0;		//清除TF1标志
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