#include<stc15f2k60s2.h>
typedef unsigned int uint;
typedef unsigned char uchar;
uchar code dig_code[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x80,0xc6,0xc0,0x86,0x8e,0xbf,0x7f,0xff};
sbit S7 = P3^0;        
sbit S6 = P3^1;        
sbit S5 = P3^2;
sbit S4 = P3^3;
uchar KeyStat[4];
uint count=0;
uchar flag=0;
void delay(uint t)
{
	while(t--);
}

void select(uchar channel)
{
	switch(channel)
	{
		case 4: 
			P2=(P2&0X1f)|0X80;//不改变P2其他IO口电平的情况下改变138译码器输入
			break;
		case 5: 
			P2=(P2&0X1f)|0Xa0;
			break;
		case 6: 
			P2=(P2&0X1f)|0Xc0;
			break;
		case 7: 
			P2=(P2&0X1f)|0Xe0;
			break;
	}
		
}

void display(uchar pos,uchar dig)
{
	delay(2000);
	P0=0XFF;
	select(6);
	P0=0X01<<pos;
	select(7);
	P0=dig_code[dig];
}

void InitSys()
{
	select(5);
	P0=0X00;
	select(4);
	P0=0XFF;
}

void Timer0Init(void)		//5毫秒@11.0592MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x00;		//设置定时初值
	TH0 = 0xEE;		//设置定时初值设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;
	EA = 1;
}


void Scan_Key_4(void)
{
	uint i;
	static uchar keybuff[] = {0xff,0xff,0xff,0xff};
	keybuff[0] = (keybuff[0]<<1)|S4;
	keybuff[1] = (keybuff[1]<<1)|S5;
	keybuff[2] = (keybuff[2]<<1)|S6;
	keybuff[3] = (keybuff[3]<<1)|S7;
	for(i = 0;i < 4;i ++)
	{
		if(keybuff[i] == 0xff)		//连续扫描8次都是1,40ms内都是弹起状态，按键已松开
		{
			KeyStat[i] = 1;
		}
		else if(keybuff[i] == 0x00)	//连续扫描8次都是0，40ms内都是按下状态，按键已按下
		{
			KeyStat[i] = 0;
		}
	}
}

void Long_Press()
{
	if(KeyStat[2]==0)
	{
		count++;
	}
	else
	{
		count=0;
	}
	if(count==200)
	{
		flag=~flag;
		count=0;
	}
}

void Ser_Timer0() interrupt 1
{
	Scan_Key_4();
	Long_Press();
}

int main()
{
	InitSys();
	P3 = 0XFF;
	Timer0Init();
	while(1)
	{
		display(0,KeyStat[0]);
		display(1,KeyStat[1]);
		if(flag)
		{
			display(2,KeyStat[2]);
		}
	}
}