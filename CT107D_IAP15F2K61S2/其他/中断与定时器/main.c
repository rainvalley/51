#include<reg52.h>
unsigned int count_t = 0; //��ʱ������
unsigned int count_f = 0; //Ƶ�ʼ���
unsigned int dat_f = 0; //Ƶ����ֵ����

void Init_Sys() //��ʼ���������漰������������������������ɲ����ġ�
{
	P2 = (P2&0X1F)|0XA0; //���ı������λ��ֵ������Ӱ������λ��ƽ��
	P0 = 0X00;
	P2 = (P2&0X1F)|0X80;
	P0 = 0XFF;
}

void Init_Timer()
{
	TMOD = 0x16; //��ʱ��1��16λ��ʱ����ʱ��0��8λ�Զ���װ������
	TL0 = 0xff;
	TH0 = 0xff;
	TH1 = (65535-50000)/256;
  TL1 = (65535-50000)%256;
	
	ET0 = 1;
	ET1 = 1;
	EA = 1;
	TR0 = 1;
	TR1 = 1;
}

void Ser_Timer0() interrupt 1
{
	count_f++;
}

void Ser_Timer1() interrupt 3
{
	TH1 = (65535-50000)/256;
  TL1 = (65535-50000)%256;
	count_t++;
	if(count_t == 20) //һ����count_f�ļ���
	{
		dat_f = count_f;
		count_f = 0;
		count_t = 0;
	}
}


/* �ⲿ�жϳ�ʼ���������
void Init_INT0()
{
	EX0 = 1; //�����ⲿ�ж�0�����ǰ���жϽṹ��
	IT0 = 1; //�����ⲿ�жϴ�����ʽ�����½���
	EA = 1; //�������ж�
}

void Init_INT1()
{
	EX1 = 1;
	IT1 = 1;
	EA = 1;
}

void Ser_INT0() interrupt 0
{
	L1 = ~L1; //ȡ��LED�ƶ�Ӧ�˿ڵ�ƽ
}

void Ser_INT1() interrupt 2
{
	L2 = ~L2;
}
*/
int main()
{
	Init_Sys();
//	Init_INT0();
//	Init_INT1();
	Init_Timer();
	while(1)
	{
		if(dat_f>10)
		{
			P0 = 0XFE;
		}
		if(dat_f>100)
		{
			P0 = 0XFC;
		}
		if(dat_f>1000)
		{
			P0 = 0XF8;
		}
		if(dat_f>10000)
		{
			P0 = 0XF0;
		}
	}
}