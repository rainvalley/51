#include<reg52.h>
typedef unsigned char uchar;
typedef unsigned int uint;
sbit LED1=P0^0;
sbit LED2=P0^1;

uint count=0;

void SelectHC138(uchar channel)
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
	}	
}

void interrupt_int()
{
	TMOD=0X01;
	TH0=(65536-50000)/256;
	TL0=(65536-50000)%256;
	ET0=1;
	EA=1;
	TR0=1;
}

void interrupt_service() interrupt 1
{
	TMOD=0X01;
	TH0=(65536-50000)/256;
	TL0=(65536-50000)%256;
	count++;
	if(count==10)
	{
		LED1=~LED1;
	}
	if(count==20)
	{
		LED2=~LED2;
		count=0;
	}
}

int main()
{
	SelectHC138(5);
	P0=0X00;
	SelectHC138(4);
	P0=0Xff; 
	SelectHC138(4);	
	interrupt_int();
	while(1)
	{
		
	}
}