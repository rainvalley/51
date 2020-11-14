#include "wave.h"
#include "intrins.h"
sbit TX=P1^0; //发射
sbit RX=P1^1;	//接收

void init_timer1(void)
{
	AUXR &= 0xBF;		//定时器时钟12T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0x00;		//设置定时初值
	TH1 = 0x00;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 0;		//定时器1暂时关闭，同时注意需要关闭T1的定时器中断，否则在无中断服务函数的情况下会造成混乱
}

void delay13us()		//@12.000MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 36;
	while (--i);
}

unsigned char get_dis()
{
	unsigned char dis=0,count=10;
	while(count--)
	{
		TX=1;delay13us();
		TX=0;delay13us();
	}
	TL1=0;
	TH1=0;
	TR1=1;
	while(!TF1&&RX);
	TR1=0;
	if(TF1)
	{
		TF1=0;
		dis=255;
	}
	else
	{
		dis=((TH1<<8)|TL1)*0.017;
	}
	return dis;
}