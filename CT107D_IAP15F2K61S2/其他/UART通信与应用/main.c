/*
2020-08-01 15��56
CT107D_IAP15F2K61S2
Baud��9600bps
*/
#include<stc15f2k60s2.h>
typedef unsigned char uchar;
typedef unsigned int uint;
void Send_Byte(uchar dat);
uchar temp;

void Select(uchar channel)
{
	switch(channel)
	{
		case 4:
			P2=(P2&0x1f)|0x80;
		break;
		case 5:
			P2=(P2&0x1f)|0xa0;
		break;
	}
}

void Init_Sys()
{
	Select(4);
	P0=0xff;
	Select(5);
	P0=0x00;
}

void Delay1000ms()		//@11.0592MHz
{
	unsigned char i, j, k;

	i = 43;
	j = 6;
	k = 203;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}


void Init_Uart(void)		//9600bps@11.0592MHz
{
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR &= 0xBF;		//��ʱ��1ʱ��ΪFosc/12,��12T
	AUXR &= 0xFE;		//����1ѡ��ʱ��1Ϊ�����ʷ�����
	TMOD &= 0x0F;		//�趨��ʱ��1Ϊ16λ�Զ���װ��ʽ
	TL1 = 0xE8;		//�趨��ʱ��ֵ
	TH1 = 0xFF;		//�趨��ʱ��ֵ
	ET1 = 0;		//��ֹ��ʱ��1�ж�
	TR1 = 1;		//������ʱ��1
	ES = 1;
	EA = 1;
}

void Ser_Uart() interrupt 4
{
	if(RI)
	{
		RI=0;
		temp=SBUF;
		Send_Byte(temp+1);
	}
}

void Send_Byte(uchar dat)
{
	SBUF=dat;
	while(TI==0);
	TI=0;
}

void Send_String(uchar *str)
{
	while(*str!='\0')
	{
		SBUF=*str++;
		while(TI==0);
		TI=0;
	}
}

int main()
{
	Init_Sys();
	Init_Uart();
	Send_String("Hello World!");
	while(1)
	{
		
	}
}