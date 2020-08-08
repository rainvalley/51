#include<reg52.h>
typedef unsigned int uint;
typedef unsigned char uchar;
sbit L1 = P0^0;
sbit S7 = P3^0;

void delay(uint t)
{
	while(t--);
}

void selecthc573()
{
	P2=(P2&0X1F)|0XA0;
	P0=0x00;
	P2=(P2&0X1F)|0X80;
	P0=0XFF;
}
//Init Timer
uint count=0;
uint mode=0;
void init_timer0()
{
	TMOD=0X01;
	TH0=(65535-100)/256;
	TL0=(65535-100)%256;
	ET0=1;
	EA=1;
}
//Service Timer
void service_timer0()	interrupt 1
{
	TH0=(65535-100)/256;
	TL0=(65535-100)%256; 
	count++;
	if(count==mode)
	{
		L1=1;
	}
	else if(count==100)
	{
		L1=0;
		count = 0;
	}
}
uchar stat = 0;
void Scankey()
{
	if(S7==0)
	{
		delay(100);
		if(S7==0)
		{
			switch(stat)
			{
		  	   case 0:L1=0;TR0=1;mode=10;stat=1;break;
			   case 1:mode=50;stat=2;break;
			   case 2:mode=90;stat=3;break;
			   case 3:L1=1;TR0=0;stat=0;break;
			}
			while(S7==0);
		}
		
	}
}

int main()
{
	selecthc573();
	init_timer0();
	while(1)
	{
	 Scankey();
	}
}