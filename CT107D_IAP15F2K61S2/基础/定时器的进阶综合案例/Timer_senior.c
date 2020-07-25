#include<reg52.h>
typedef unsigned char uchar;
typedef unsigned int uint;
uchar code dig_code[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x80,0xc6,0xc0,0x86,0x8e,0xbf,0x7f};
uint count=0;
sbit S5=P3^2;
sbit S4=P3^3;
uint m=0;
uint s=0;
uint h=0;
uint ms=0;

void delay(uint t)
{
	while(t--);
}

void SelectHC138(uchar n)
{
	switch(n)
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
	}
}

void displaybit(uchar num,uchar pos)
{
	SelectHC138(6);	
	P0=0X01<<pos;
	SelectHC138(7);
	P0=dig_code[num];		
}

void display()
{
	displaybit(s%10,7);
	delay(500);
	displaybit(s/10,6);
	delay(500);
	displaybit(16,5);
	delay(500);
	displaybit(m%10,4);
	delay(500);
	displaybit(m/10,3);
	delay(500);
	displaybit(16,2);
	delay(500);
	displaybit(h%10,1);
	delay(500);
	displaybit(h/10,0);
	delay(500);
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
	ms++;
	if(ms==20)
	{
		s++;
		ms=0;
		if(s==60)
		{
			m++;
			s=0;
			if(m==60)
			{
				h++;
				m=0;
			}
			if(h==24)
			{
				h=0;
			}
		}
		

	}
}

void scankey()
{
	while(S4==0)
	{
		delay(100);
		while(S4==0)
		{
			TR0=~TR0;
			while(S4==0)
			{
				display();
			}
			
		}
	}
	while(S5==0)
	{
		delay(100);
		while(S5==0)
		{
			TR0=0;
			m=0;
			s=0;
			ms=0;
			while(S5==0)
			{
				display();
			}
		}
	}		
}

int main()
{
	SelectHC138(5);
	P0=0X00;
	SelectHC138(4);
	P0=0xff;
	SelectHC138(4);	
	interrupt_init();
	while(1)
	{
		display();
		scankey();
	}
}