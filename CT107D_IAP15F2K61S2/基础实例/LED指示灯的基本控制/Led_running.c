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

void Delay(uint t)
{
	while(t--);//延时函数
}

void ledrunning()
{
	uchar i;
	for(i=0;i<3;i++)
	{
	HC138_C=1;
	HC138_B=0;
	HC138_A=0;
	P0=0xff;//熄灭所有LED灯
	Delay(30000000);
	P0=0x00;//点亮所有LED小灯	
	Delay(30000000);
	}
	for(i=1;i<=8;i++)
	{
		P0=0xff<<i;//左移函数，每次循环均将已点亮小灯向左移动一位
		Delay(30000000);
	}
	for(i=1;i<=8;i++)
	{
		P0=~(0xff<<i);//取反
		Delay(30000000);
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