#include<reg52.h>
typedef unsigned char uchar;
typedef unsigned int uint;
sbit L1=P0^0;
sbit L8=P0^7;

void Delay(uint t)
{
	while(t--);
	while(t--);
	while(t--);

}

void SelectHC573(uchar c)
{
	if(c==4)
	P2=(P2&0x1f) | 0x80;
	else if(c==5)
	P2=(P2&0x1f) | 0xa0;
}

void working()
{	
	SelectHC573(4);
	L1=0;
	Delay(60000);
	L1=1;
	Delay(60000);
}

void Init_INT0()
{
	IT0=1;
	EX0=1;
	EA=1;

}

void SerINT0() interrupt 0
{
	L8=0;
	Delay(60000);
	Delay(60000);
	Delay(60000);
	Delay(60000);
	Delay(60000);
	Delay(60000);
	L8=1;
}

void main()
{
	SelectHC573(5);
	P0=0x00;
	SelectHC573(4);
	P0=0XFF;
	Init_INT0();
	while(1)
	{
	 	working();	
	}
}