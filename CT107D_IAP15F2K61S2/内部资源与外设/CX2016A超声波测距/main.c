#include <stc15f2k60s2.h>
#include "wave.h"
typedef unsigned char uchar;
typedef unsigned int uint;
uchar dig_code[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xc1,0x8c,0xc8,0xff};
uchar buff_tube[]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
uchar dis,flag_100ms,buff_uart;
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
		default:
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
	buff_tube[0]=dig_code[dis/100];
	buff_tube[1]=dig_code[dis%100/10];
	buff_tube[2]=dig_code[dis%10];
	buff_tube[3]=dig_code[13];
	buff_tube[4]=dig_code[13];
	buff_tube[5]=dig_code[13];
	buff_tube[6]=dig_code[13];
	buff_tube[7]=dig_code[13];
}

void init_timer0(void)		//1毫秒@12.000MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x18;		//设置定时初值
	TH0 = 0xFC;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	EA = 1;
	ET0 = 1;
}

void ser_timer0() interrupt 1
{
	static uint count_100ms;
	count_100ms++;
	if(count_100ms>=100)
	{
		flag_100ms=1;
		count_100ms=0;
	}
	set_display();
	display();
}

void init_uart(void)		//9600bps@12.000MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x01;		//串口1选择定时器2为波特率发生器
	AUXR &= 0xFB;		//定时器2时钟为Fosc/12,即12T
	T2L = 0xE6;		//设定定时初值
	T2H = 0xFF;		//设定定时初值
	AUXR |= 0x10;		//启动定时器2
	EA = 1;
	ES = 1;
	IP = 0x10; //设置串口优先级，避免大量发送/接受时候的误码
}


void send_string(uchar *str)
{
	while(*str!='\0')
	{
		SBUF=*str;
		while(TI==0);
		TI=0;
		str++;
	}
}

void send_byte(uchar dat)
{
	SBUF=dat;
	while(TI==0);
	TI=0;
}

void ser_uart() interrupt 4
{
	if(RI)
	{
		send_byte(dis);
		RI=0;
	}
}

int main()
{
	init_sys();
	init_timer0();
	init_timer1();
	init_uart();
	send_string("Hello World!\r\n");
	while(1)
	{	
		if(flag_100ms)
		{
			flag_100ms=0;
			dis=get_dis();
			//send_byte(dis);
		}
	}
}
