#include<stc15f2k60s2.h>
typedef unsigned int uint;
typedef unsigned char uchar;
uchar code dig_code[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x80,0xc6,0xc0,0x86,0x8e,0xbf,0x7f,0xff};
#define R1_ P3^0
#define R2_ P3^1
#define R3_ P3^2
#define R4_ P3^3
#define C1_ P4^4
#define C2_ P4^2
#define C3_ P3^5
#define C4_ P3^4
#define KEY R1_|(R2_<<1)|(R3_<<2)|(R4_<<3)|(C1_<<4)|(C2_<<5)|(C3_<<6)|(C4_<<7)
sbit R1 = P3^0;
sbit R2 = P3^1;
sbit R3 = P3^2;
sbit R4 = P3^3;
sbit C1 = P4^4;
sbit C2 = P4^2;
sbit C3 = P3^5;
sbit C4 = P3^4;
uchar trg_row=0;
uchar trg_column=0;
uchar cont_row=0;
uchar cont_column=0;

void Delay(uint t)
{
	while(t--);
}

void Select(uchar channel)
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

void Display(uchar pos,uchar dig)
{
	Delay(200);
	P0=0XFF;
	Select(6);
	P0=0X01<<pos;
	Select(7);
	P0=dig_code[dig];
}

void Init_Sys()
{
	Select(5);
	P0=0X00;
	Select(4);
	P0=0XFF;
}

void Timer0Init(void)		//50毫秒@11.0592MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x00;		//设置定时初值
	TH0 = 0xDC;		//设置定时初值设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;
	EA = 1;
}

void Scan_Key_16(void)
{
	uchar read_row,read_column;
	
	R1=R2=R3=R4=1;
	C1=C2=C3=C4=0;
	read_row = KEY^(0x0f);
	trg_row = read_row&(trg_row^cont_row);
	cont_row = read_row;
	
	R1=R2=R3=R4=0;
	C1=C2=C3=C4=1;
	read_column = KEY^(0xf0);
	trg_column = read_column&(trg_column^cont_column);
	cont_column = read_column;
}

void Ser_Timer0() interrupt 1
{
	Scan_Key_16();
}
int main()
{
	Init_Sys();
	P3 |= 0X0F;
	Timer0Init();
	while(1)
	{
		Display(0,cont_row);
		Display(1,cont_column);
	}
}