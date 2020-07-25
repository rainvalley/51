/*
2020-07-14 10:17
CT107D_IAP15F2K61S2
*/
#include<stc15f2k60s2.h>
sbit LED=P0^0;
unsigned char t_count=0;

void init_sys()
{
	P2=(P2&0x1F)|0xA0;	//Ñ¡Ôñµ½Y5¶Ë¿Ú
	P0=0x00;  //ÅäÖÃ·äÃùÆ÷
	P2=(P2&0x1F)|0x80;
	P0=0xff;	//ÅäÖÃLEDµÆÏ¨Ãğ
}

void init_timer()
{
	TMOD=0X01;
	TH0=(65536-50000)/256;
	TL0=(65536-50000)%256;
	ET0=1;
	EA=1;
	TR0=1;
}

void ser_timer()interrupt 1
{
	TH0=(65536-50000)/256;
	TL0=(65536-50000)%256;
	t_count++;
	if(t_count==80)
	{
		LED=~LED;
	}
	if(t_count==100)
	{
		LED=~LED;
		t_count=0;
	}
}

int main()
{
	init_sys();
	init_timer();
	while(1);
}


