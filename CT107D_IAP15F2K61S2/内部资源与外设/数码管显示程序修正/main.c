#include<stc15f2k60s2.h>
typedef unsigned char uchar;
typedef unsigned int uint;
uchar code dig_code[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf,0xff,0xc6};
uchar buff_tube[]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
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
}

void display(void)
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

void set_tube()
{
	buff_tube[0]=dig_code[1];
	buff_tube[1]=dig_code[2];
	buff_tube[2]=dig_code[3];
	buff_tube[3]=dig_code[4];
	buff_tube[4]=dig_code[5];
	buff_tube[5]=dig_code[6];
	buff_tube[6]=dig_code[7];
	buff_tube[7]=dig_code[8];
}

void init_timer0(void)		//1毫秒@11.0592MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0xCD;		//设置定时初值
	TH0 = 0xD4;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;
	EA = 1;
}

void ser_timer0() interrupt 1
{
	static uchar count=0;
	count++;
	if(count==2)
	{
		display();
		count=0;
	}
}

void main()
{
	init_sys();
	init_timer0();
	while(1)
	{
		set_tube();
	}
}