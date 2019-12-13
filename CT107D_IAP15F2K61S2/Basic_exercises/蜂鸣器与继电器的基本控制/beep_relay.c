#include<reg51.h>
typedef unsigned char uchar;
typedef unsigned int uint;
sbit HC138_A=P2^5;
sbit HC138_B=P2^6;
sbit HC138_C=P2^7;

void Delay(uint t)
{
	while(t--);	
}

void Initsys()
{
	HC138_C=1;
	HC138_B=0;
	HC138_A=1;//Y5����͵�ƽ
	P0=0X00;//��������P0�˿ڵ�ƽ��ʼ��
}

void beep()
{
	HC138_C=1;
	HC138_B=0;
	HC138_A=1;//Y5����͵�ƽ
	P0=0x40;//��Ӧ��Ӧ0100 0000������P06_D7_Q7_IN7_OUT7_N BUZZ
	Delay(360000);
	P0=0X00;	
}

void relay()
{
	HC138_C=1;
	HC138_B=0;
	HC138_A=1;//Y5����͵�ƽ	
	P0=0x10;//��Ӧ0001 0000������P04_D5_Q5_IN5_OUT5_N RELAY
	Delay(360000);
	P0=0X00;
}



void leds()
{
	uchar i;
	HC138_C=1;
	HC138_B=0;
	HC138_A=0;//Y4����͵�ƽ	
	for(i=1;i<=8;i++)
	{
		P0=0XFF<<i;
		Delay(360000);
	}
	for(i=1;i<=8;i++)
	{
		P0=~(0XFF<<i);
		Delay(360000);
	}
}

void main()
{
	Initsys();
	while(1)
	{
		leds();
		beep();
		relay();
	}
}