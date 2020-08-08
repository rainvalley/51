#include<reg52.h>
typedef unsigned int uint;
typedef unsigned char uchar;
sbit HC138_A=P2^5;
sbit HC138_B=P2^6;
sbit HC138_C=P2^7;//138����������˶���
void Init()
{
	HC138_C=1;
	HC138_B=0;
	HC138_A=1;
	P0=0X00;//��ʼ��������Դ���رշ�������̵���
}

void Delay(uchar t)//��ʱ����
{
	uchar i = 255;
	while(t--)
	{
		while(i--);
	}
}

void ledrunning()
{
	uchar i;
	for(i=0;i<3;i++)
	{
		HC138_C=1;
		HC138_B=0;
		HC138_A=0;
	}
	for(i=0;i<8;i++)
	{
		P0=0xff<<i;//���ƺ�����ÿ��ѭ�������ѵ���С�������ƶ�һλ
		Delay(200);
	}
	for(i=0;i<8;i++)
	{
		P0=~(0xff<<i);//ȡ��
		Delay(200);
	}
 
}


void main()
{
	Init();
	while(1)
	{
		ledrunning();
	}
}