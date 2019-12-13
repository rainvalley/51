#include<reg51.h>
typedef unsigned char uchar;
typedef unsigned int uint;
sfr P4=0xc0;
sbit R1=P3^0;
sbit R2=P3^1;
sbit R3=P3^2;
sbit R4=P3^3;
sbit C1=P3^4;
sbit C2=P3^5;
sbit C3=P4^2;
sbit C4=P4^4;//Œª∂®“Â
uchar num;
uchar code dig_code[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};


void SelectHC573(uchar channel)
{
	switch(channel)
	{
		case 4:
		P2=(P2&0x1f)|(0x80);
		break;
		case 5:
		P2=(P2&0x1f)|(0xa0);
		break;
		case 6:
		P2=(P2&0x1f)|(0xc0);
		break;
		case 7:
		P2=(P2&0x1f)|(0xe0);
		break;
	}
}

void Display(uchar value)
{
	SelectHC573(6);
	P0=0x01;
	SelectHC573(7);
	P0=value;
}

void Scan()
{
	R1=0;
	R2=R3=R4=1;
	C1=C2=C3=C4=1;
	if(C1==0)
	{
		while(C1==0);
		num=0;
		Display(dig_code[num]);
	}
	else if(C2==0)
	{
		while(C2==0);
		num=1;
		Display(dig_code[num]);
	}
	else if(C3==0)
	{
		while(C3==0);
		num=2;
		Display(dig_code[num]);
	}
	else if(C4==0)
	{
		while(C4==0);
		num=3;
		Display(dig_code[num]);
	}


	R2=0;
	R1=R3=R4=1;
	C1=C2=C3=C4=1;
	if(C1==0)
	{
		while(C1==0);
		num=4;
		Display(dig_code[num]);
	}
	else if(C2==0)
	{
		while(C2==0);
		num=5;
		Display(dig_code[num]);
	}
	else if(C3==0)
	{
		while(C3==0);
		num=6;
		Display(dig_code[num]);
	}
	else if(C4==0)
	{
		while(C4==0);
		num=7;
		Display(dig_code[num]);
	}

	R3=0;
	R2=R1=R4=1;
	C1=C2=C3=C4=1;
	if(C1==0)
	{
		while(C1==0);
		num=8;
		Display(dig_code[num]);
	}
	else if(C2==0)
	{
		while(C2==0);
		num=9;
		Display(dig_code[num]);
	}
	else if(C3==0)
	{
		while(C3==0);
		num=10;
		Display(dig_code[num]);
	}
	else if(C4==0)
	{
		while(C4==0);
		num=11;
		Display(dig_code[num]);
	}
}
void main()
{
	SelectHC573(5);
	P0=0x00;
	while(1)
	{
		Scan();
	}
}