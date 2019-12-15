#include<reg52.h>
typedef unsigned char uchar;
typedef unsigned int uint;
sfr AUXR=0X8E;
void SelectHC138(uint ch)
{
	switch(ch)
	{
	case 0:
	P2=(P2&0X1F) | 0x00;
	break;
	case 4:
	P2=(P2&0X1F) | 0x80;
	break;
	case 5:
	P2=(P2&0X1F) | 0xa0;
	break;
	case 6:
	P2=(P2&0X1F) | 0xc0;
	break;
	case 7:
	P2=(P2&0X1F) | 0xe0;
	break;
	}
}

void Init_sys()
{
	SelectHC138(5);
	P0=0X00;
	SelectHC138(4);
	P0=0Xff;
	SelectHC138(0);
}

void Send_char(uchar dat)
{
	SBUF=dat;
	while(TI==0);
	TI=0;
}

void Send_string(uchar *str)
{
	while(*str!='\0')
	{
		Send_char(*str++);
	}
}

void Init_uart()
{
   TMOD=0X20;
   TH1=0XFD;
   TL1=0XFD;
   TR1=1;
   AUXR=0X00;
   SCON=0X50;
   ES=1;
   EA=1;
}

void Ser_uart()
{
   if(RI==1)
   {
   }
}

void main()
{
	Init_sys();
	Init_uart();
	Send_string("Hello World!");
	while(1);
}