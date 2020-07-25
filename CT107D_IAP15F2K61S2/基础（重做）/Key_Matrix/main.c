#include<reg52.h>
typedef unsigned char uchar;
typedef unsigned int uint;
sfr P4=0xc0;
sbit R1=P3^0;
sbit R2=P3^1;
sbit R3=P3^2;
sbit R4=P3^3;
sbit C4=P3^4;
sbit C3=P3^5;
sbit C2=P4^2;
sbit C1=P4^4;
uchar code dig_code[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x80,0xc6,0xc0,0x86,0x8e,0xbf,0x7f};

void select(uchar channel)
{
	switch(channel)
	{
		case 4: P2=(P2&0X1F)|0x80;break;
		case 5: P2=(P2&0X1F)|0XA0;break;
		case 6: P2=(P2&0X1F)|0XC0;break;
		case 7: P2=(P2&0X1F)|0XE0;break;
	}
}
void init()
{
	select(4);
	P0=0XFF;
	select(5);
	P0=0X00;
}
void delay(uint t)
{
	while(t--);
}

void display(uchar num)
{
	select(6);
	P0=0X01;
	select(7);
	P0=dig_code[num];
}

void key_matrix()
{
	R1=0;
	R2=R3=R4=1;
	C1=C2=C3=C4=1;
	if(C1==0)
	{
		delay(500);
		while(C1==0);
		display(0);
	}
	
	if(C2==0)
	{
		delay(500);
		while(C2==0);
		display(1);
	}
	
	if(C3==0)
	{
		delay(500);
		while(C3==0);
		display(2);
	}
	
	if(C4==0)
	{
		delay(500);
		while(C4==0);
		display(3);
	}
	//
	R2=0;
	R1=R3=R4=1;
	C1=C2=C3=C4=1;
	if(C1==0)
	{
		delay(500);
		while(C1==0);
		display(4);
	}
	
	if(C2==0)
	{
		delay(500);
		while(C2==0);
		display(5);
	}
	
	if(C3==0)
	{
		delay(500);
		while(C3==0);
		display(6);
	}
	
	if(C4==0)
	{
		delay(500);
		while(C4==0);
		display(7);
	}
	//
	R3=0;
	R2=R1=R4=1;
	C1=C2=C3=C4=1;
	if(C1==0)
	{
		delay(500);
		while(C1==0);
		display(8);
	}
	
	if(C2==0)
	{
		delay(500);
		while(C2==0);
		display(9);
	}
	
	if(C3==0)
	{
		delay(500);
		while(C3==0);
		display(10);
	}
	
	if(C4==0)
	{
		delay(500);
		while(C4==0);
		display(11);
	}
	//
	R4=0;
	R2=R3=R1=1;
	C1=C2=C3=C4=1;
	if(C1==0)
	{
		delay(500);
		while(C1==0);
		display(12);
	}
	
	if(C2==0)
	{
		delay(500);
		while(C2==0);
		display(13);
	}
	
	if(C3==0)
	{
		delay(500);
		while(C3==0);
		display(14);
	}
	
	if(C4==0)
	{
		delay(500);
		while(C4==0);
		display(15);
	}
}


void main()
{
	init();
	while(1)
	{
		key_matrix();
	}
}