#include<reg52.h>
typedef unsigned int uint;
typedef unsigned char uchar;
uchar code dig_code[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x80,0xc6,0xc0,0x86,0x8e,0xbf,0x7f,0xff};
sbit S7 = P3^0;        
sbit S6 = P3^1;        
sbit S5 = P3^2;        
sbit S4 = P3^3;
uchar mode=0;
uchar times=0;
void delay(uint t)
{
	while(t--);
	while(t--);
}

void select(uchar channel)
{
	switch(channel)
	{
		case 4: 
			P2=(P2&0X1f)|0X80;
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

void display(uchar dig)
{
	select(6);
	P0=0X01;
	select(7);
	P0=dig_code[dig];
}

void scankey_signal()
{
	if(S7==0)
	{
		delay(100);
		if(S7==0)
		{
			while(S7==0);
			display(7);
		}
	}
	
	if(S6==0)
	{
		delay(100);
		if(S6==0)
		{
			display(6);
			while(S6==0);
			display(18);
		}
	}
	
	if(S5==0)
	{
		delay(100);
		if(S5==0)
		{
			display(5);
			while(S5==0);
			times+=1;
			if(times==10)
			{
				times=0;
			}
			display(times);
		}
	}
	
	if(S4==0)
	{
		delay(100);
		if(S4==0)
		{
			while(S4==0);
			display(4);
		}
	}
}


int main()
{
	while(1)
	{
		scankey_signal();
	}
}