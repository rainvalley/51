#include <stc15f2k60s2.h>
#include <intrins.h>
typedef unsigned char uchar;
typedef unsigned int uint;
uchar code dig_code[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf,0xff,0xc6};
uchar buff_tube[]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
uchar flag_200ms=0;
float distance=0;
sbit trg=P1^0;
sbit echo=P1^1;

void select(uchar channel)
{
	switch(channel)
	{
		case 4:
			P2=(P2&0x1f)|0x80;
			break;
		case 5:
			P2=(P2&0x1f)|0xa0;
			break;
		case 6:
			P2=(P2&0x1f)|0xc0;
			break;
		case 7:
			P2=(P2&0x1f)|0xe0;
			break;
		case 0:
			P2=(P2&0x1f);
			break;
	}
}

void init_sys()
{
	select(4);
	P0=0xff;
	select(5);
	P0=0x00;
	select(0);
	trg=0;
}

void display()
{
	static uchar index=0;
	select(7);
	P0=0xff;
	select(6);
	P0=0x01<<index;
	select(7);
	P0=buff_tube[index];
	select(0);
	index++;
	index&=0x07;
}

void set_display()
{
	buff_tube[0]=dig_code[(uchar)distance/100];
	buff_tube[1]=dig_code[(uchar)distance%100/10];
	buff_tube[2]=dig_code[(uchar)distance%10];
	buff_tube[3]=0xff;
	buff_tube[4]=0xff;
	buff_tube[5]=0xff;
	buff_tube[6]=0xff;
	buff_tube[7]=0xff;
}

void delay_10us()		//@11.0592MHz
{
	unsigned char i;

	_nop_();
	i = 25;
	while (--i);
}

void init_timer0(void)
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x00;		//设置定时初值
	TH0 = 0x00;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 0;
	ET0 = 1;
	EA = 1;
}

void sr04_measure()
{
	uint time=0;
	trg=1; 
	delay_10us();
	trg=0; //拉高trg保持10us以上触发测距
	while(echo==0); //等待echo变为高电平
	TR0=1;
	while(echo==1&&TF0==0); //等待echo变为低电平
	TR0=0;
	if(TF0==1)
	{
		TF0=0;
		TH0=TL0=0;
		distance=999;
	}
	else
	{
		time=TH0*256+TL0;
		TH0=TL0=0;
		distance=(time*1.7)/100;
	}
}

void init_timer1(void)		//1毫秒@11.0592MHz
{
	AUXR &= 0xBF;		//定时器时钟12T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0x66;		//设置定时初值
	TH1 = 0xFC;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时
	ET1 = 1;
	EA = 1;
}

void ser_timer1() interrupt 3
{
	static count_2ms,count_200ms;
	count_2ms++;
	count_200ms++;
	if(count_2ms>=2)
	{
		count_2ms=0;
		set_display();
		display();
	}
	if(count_200ms>=200)
	{
		count_200ms=0;
		flag_200ms=1;
	}
}

void main()
{
	init_sys();
	init_timer0();
	init_timer1();
	while(1)
	{
		if(flag_200ms)
		{
			flag_200ms=0;
			sr04_measure();
		}
	}
}
