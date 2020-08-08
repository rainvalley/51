/*
2020-08-01 15��56
CT107D_IAP15F2K61S2
Baud��9600bps
*/
#include<stc15f2k60s2.h>
typedef unsigned char uchar;
typedef unsigned int uint;

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
	}
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
	Send_String("Last week I went to the theatre. I had a very good seat. The play was very interesting. I did not enjoy it. A young man and a young woman were sitting behind me. They were talking loudly. I got very angry. I could not hear the actors. I turned round. I looked at the man and the woman angrily. They did not pay any attention. In the end, I could not bear it. I turned round again. ��I can't hear a word!�� I said angrily.��It's none of your business, �� the young man said rudely. ��This is a private conversation!��");
	while(1);
}