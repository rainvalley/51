#include<reg52.h>
#include<intrins.h>
sfr AUXR=0x8e;
typedef unsigned int uint;
typedef unsigned char uchar;
uchar temprecv;
void Init_uart()
{
	TMOD=0X20;
	TH1=0XFD;
	TL1=0XFD;
	AUXR=0X00;
	TR1=1;
	SCON=0X50;
	ES=1;
	EA=1;
}

void Delay1ms()		//@11.0592MHz
{
	unsigned char i, j;

	_nop_();
	_nop_();
	_nop_();
	i = 11;
	j = 190;
	do
	{
		while (--j);
	} while (--i);
}

void delay(uint t)
{
	while(t--)
	{
		Delay1ms();
	}
}



void Send(uchar bytes)
{
	SBUF=bytes;
	while(TI==0);
	TI=0;
}

void uart_ser() interrupt 4
{
	if(RI==1)
	{
		RI=0;
		temprecv=SBUF;
		Send(temprecv+1);
	}
}

int main()
{
	Init_uart();
	delay(250);
	Send(0xff);
	Send(0x00);	
	while(1);
}
