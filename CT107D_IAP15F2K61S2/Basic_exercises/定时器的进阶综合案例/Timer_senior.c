#include<reg52.h>
typedef unsigned char uchar;
typedef unsigned int uint;
char dig[10]=(0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90);
uint count=0;

SelectHC138(uchar n)
{
	switch(n)
	{
		case 4:
		P0=(P0&0x1f)|0x80;
		break;
		case 5:
		P0=(P0&0x1f)|0xa0;
		break;
		case 6:
		P0=(P0&0x1f)|0xc0;
		break;
		case 7:
		P0=(P0&0x1f)|0xe0;
	}
}

void display(uint t)
{
		
}

void interrupt_init()
{
	TMOD=0X01;
	TH0=(65536-50000)/256;
	TL0=(65536-50000)%256;
	EA=1;
	ET0=1;
	TR0=1;
}

void interrupt_service() interrupt 1
{
	TH0=(65536-50000)/256;
	TL0=(65536-50000)%256;
	count++;
	Display(count);
}

int main()
{
	SelectHC138(5);
	P0=0X00;
	interrupt_init();
	while(1);
}