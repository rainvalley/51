#include "wave.h"
#include "intrins.h"
sbit TX=P1^0; //����
sbit RX=P1^1;	//����

void init_timer1(void)
{
	AUXR &= 0xBF;		//��ʱ��ʱ��12Tģʽ
	TMOD &= 0x0F;		//���ö�ʱ��ģʽ
	TL1 = 0x00;		//���ö�ʱ��ֵ
	TH1 = 0x00;		//���ö�ʱ��ֵ
	TF1 = 0;		//���TF1��־
	TR1 = 0;		//��ʱ��1��ʱ�رգ�ͬʱע����Ҫ�ر�T1�Ķ�ʱ���жϣ����������жϷ�����������»���ɻ���
}

void delay13us()		//@12.000MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 36;
	while (--i);
}

unsigned char get_dis()
{
	unsigned char dis=0,count=10;
	while(count--)
	{
		TX=1;delay13us();
		TX=0;delay13us();
	}
	TL1=0;
	TH1=0;
	TR1=1;
	while(!TF1&&RX);
	TR1=0;
	if(TF1)
	{
		TF1=0;
		dis=255;
	}
	else
	{
		dis=((TH1<<8)|TL1)*0.017;
	}
	return dis;
}