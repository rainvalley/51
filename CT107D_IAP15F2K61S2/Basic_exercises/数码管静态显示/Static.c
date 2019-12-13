#include<reg51.h>
typedef unsigned char uchar;
typedef unsigned int uint;

uchar code dig_code[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};

void Init_HC138(uchar n)
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
		break;

	}
}

void Delay(uint t)
{
	while(t--);
}


void Init_sys()
{
	Init_HC138(5);
	P0=0x00;
}//初始化系统

void showdata(uchar dat,uchar pos)
{
	Init_HC138(6);
	P0=0X01<<pos;
	Init_HC138(7);
	P0=dig_code[dat];
}

void main()
{
	uchar i,j;
	Init_sys();
	while(1)
	{

		Init_HC138(7);
		for(i=0;i<8;i++)
		{
			for(j=0;j<10;j++)
			{
				showdata(j,i);
				Delay(3600000);
				P0=0x00;
			}
			
		}
	}
}