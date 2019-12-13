#include<reg51.h>
void Display_Dynamic();
typedef unsigned char uchar;
typedef unsigned int uint;
uchar code dig_code[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x80,0xc6,0xc0,0x86,0x8e,0xbf,0x7f};
uchar i=1;

void Delay(uint t)
{
	while(t--);
}//普通延迟函数

void Delay0(uint t)
{
	while(t--)
	{
		 Display_Dynamic();
	}
}//特殊延迟函数，延迟时显示数据。

void selectHC573(uchar channel)
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

void DisplaySMG_Bit(uchar value,uchar pos)
{
	selectHC573(6);
	P0=0X01<<pos;
	selectHC573(7);
	P0=value;
}

void Display_Dynamic()
{
	 DisplaySMG_Bit(dig_code[2],0);
	 Delay(500);
	 P0=0X00;
	 DisplaySMG_Bit(dig_code[0],1);
	 Delay(500);
	 P0=0X00;
	 DisplaySMG_Bit(dig_code[1],2);
	 Delay(500);
	 P0=0X00;
	 DisplaySMG_Bit(dig_code[9],3);
	 Delay(500);
	 P0=0X00;
	 DisplaySMG_Bit(dig_code[16],4);
	 Delay(500);
	 P0=0X00;
	 DisplaySMG_Bit(dig_code[16],5);
	 Delay(500);
	 P0=0X00;
	 DisplaySMG_Bit(dig_code[i/10],6);
	 Delay(500);
	 P0=0X00;
	 DisplaySMG_Bit(dig_code[i%10],7);
	 Delay(500);
	 P0=0X00;
}

void main()
{
	while(1)
	{
		 Display_Dynamic();
		 i++;
		 if(i>12)
		 {
		 	i=1;
		 }
		 Delay0(200);
	}
}