#include<stc15f2k60s2.h>
typedef unsigned char uchar;
typedef unsigned int uint;
sbit step = P0^0;

void select(uchar channel)
{
	switch(channel)
	{
		case 4:
			P2=(P2&0x1f)|0x80;
			break;
		case 5:
			P2=(P2&0x1f)|0xa0;
			break;
		case 0:
			P2=P2&0x1f;
			break;
	}
}

void init_sys()
{
	select(4);
	P0=0xff;
	select(5);
	P0=0x00;
}

void init_timer0(void)		//100us@11.0592MHz
{
	AUXR &= 0x7F;		//��ʱ��ʱ��12Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TL0 = 0xA4;		//���ö�ʱ��ֵ
	TH0 = 0xFF;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
	ET0 = 1;
	EA = 1;
}

void ser_timer0() interrupt 1
{
	static uint s_count=0;
	static uint s_step=0;
	s_count++;
	if(s_count==100)
	{
		s_step++;
	}
	if(s_step==100)
	{
		s_step=0;
	}
	if(s_count==s_step)
	{
		select(4);
		P0=0xff;
	}
	if(s_count==100)
	{
		select(4);
		P0=0x00;
		s_count=0;
	}
}

void init_timer1(void)		//0.5����@11.0592MHz
{
	AUXR &= 0xBF;		//��ʱ��ʱ��12Tģʽ
	TMOD &= 0x0F;		//���ö�ʱ��ģʽ
	TL1 = 0x33;		//���ö�ʱ��ֵ
	TH1 = 0xFE;		//���ö�ʱ��ֵ
	TF1 = 0;		//���TF1��־
	TR1 = 1;		//��ʱ��1��ʼ��ʱ
	ET1 = 1;
	EA = 1;
}

void ser_timer1() interrupt 3
{
	static uchar s_sg=0;
	s_sg++;
	if(s_sg==4)
	{
		step=1;
	}
	if(s_sg==40)
	{
		step=0;
		s_sg=0;
	}
}

int main()
{
	init_sys();
	//init_timer0();
	init_timer1();
	P0M0=0x01;
	P0M1=0x00;
	while(1);
}