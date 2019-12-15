#include<reg52.h>
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

void Send(uchar bytes)
{
	SBUF=bytes;
	while(TI=0);
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
	Send(0xff);
	Send(0x00);	
	while(1);
}
