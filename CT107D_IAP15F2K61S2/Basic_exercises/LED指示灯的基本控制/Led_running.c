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

void Delay(uint t)
{
	while(t--);//��ʱ����
}

void ledrunning()
{
	uchar i;
	for(i=0;i<3;i++)
	{
	HC138_C=1;
	HC138_B=0;
	HC138_A=0;
	P0=0xff;//Ϩ������LED��
	Delay(30000000);
	P0=0x00;//��������LEDС��	
	Delay(30000000);
	}
	for(i=1;i<=8;i++)
	{
		P0=0xff<<i;//���ƺ�����ÿ��ѭ�������ѵ���С�������ƶ�һλ
		Delay(30000000);
	}
	for(i=1;i<=8;i++)
	{
		P0=~(0xff<<i);//ȡ��
		Delay(30000000);
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