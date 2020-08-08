#include<reg52.h>
typedef unsigned int uint;
typedef unsigned char uchar;
sbit HC138_A=P2^5;
sbit HC138_B=P2^6;
sbit HC138_C=P2^7;//138译码器输出端定义
void Init()
{
	HC138_C=1;
	HC138_B=0;
	HC138_A=1;
	P0=0X00;//初始化板上资源，关闭蜂鸣器与继电器
}

void Delay(uchar t)//延时函数
{
	uchar i = 255;
	while(t--)
	{
		while(i--);
	}
}

void ledrunning()
{
	uchar i;
	for(i=0;i<3;i++)
	{
		HC138_C=1;
		HC138_B=0;
		HC138_A=0;
	}
	for(i=0;i<8;i++)
	{
		P0=0xff<<i;//左移函数，每次循环均将已点亮小灯向左移动一位
		Delay(200);
	}
	for(i=0;i<8;i++)
	{
		P0=~(0xff<<i);//取反
		Delay(200);
	}
 
}


void main()
{
	Init();
	while(1)
	{
		ledrunning();
	}
}