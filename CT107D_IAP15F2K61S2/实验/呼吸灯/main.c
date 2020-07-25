#include<stc15.h>
typedef unsigned int uint;
typedef unsigned char uchar;

uchar duty=0;
uchar count=0;
uchar i=0;

void Delay10ms()		//@11.0592MHz
{
	unsigned char i, j;

	i = 108;
	j = 145;
	do
	{
		while (--j);
	} while (--i);
}

void select(uchar channel)
{
	switch(channel)
	{
		case 4: P2=(P2&0X1F)|0X80;break;
		case 5: P2=(P2&0X1F)|0XA0;break;
		case 6: P2=(P2&0X1F)|0XC0;break;
		case 7: P2=(P2&0X1F)|0XE0;break;
	}
}

void initsys()
{
	select(5);
	P0=0X00;
	select(4);
	P0=0XFF;
}

void Timer0Init(void)		//10微秒@11.0592MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x91;		//设置定时初值
	TH0 = 0xFF;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	
	EA=1;
	ET0=1;
}



void timerser() interrupt 1
{
	count++;
	if(count==duty)
	{
		P0=0Xff<<i;
	}
	else if(count==100)
	{
		P0=~(0Xff<<i);
		count=0;
	}
}


void main()
{
	initsys();
	Timer0Init();
	while(1)
	{
		Delay10ms();
		duty=duty+1;
		i++;
		if(duty==100)
		{
			duty=0;
			i++;
		}
		if(i==8)
		{
			i=0;
		}
	}
}