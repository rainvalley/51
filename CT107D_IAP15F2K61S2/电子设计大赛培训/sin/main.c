/*
DAC:PCF8591 Over IIC
CT107D_IAP15F2K61S2
IRC Freq:12MHz
*/
#include<stc15f2k60s2.h>
#include"iic.h"
#include <intrins.h>
#define len 60
typedef unsigned int uint;
typedef unsigned char uchar;

sbit S4=P3^3;
sbit S5=P3^2;
uchar sin_tab[len]={
	128,141,154,167,180,191,203,213,223,231,238,244,249,253,255,255,255,253,249,245,238,231,223,213,203,192,180,167,154,141,128,114,101,88,76,64,53,42,33,24,17,11,6,2,0,0,0,2,6,10,16,24,32,41,52,63,75,87,100,114
};
uchar sin_status=1; //������Ӧ�Ĳ�����
float sin_range=1; //������Ӧ�Ĳ�����
float freq; //��Ƶ��
float range; //������
uchar flag=0;
	
void init_sys()
{
	P2=(P2&0x1F)|0xA0;	//ѡ��Y5�˿�
	P0=0x00;  //���÷�����
	P2=(P2&0x1F)|0x80;
	P0=0xff;	//����LED��Ϩ��
}

void Timer0Init()		//65����@11.0592MHz
{
	float time=1000.0/freq*1000; //ת��Ϊ��ʱ�ۼӵ�Ԫ�Ķ�Ӧ����
	AUXR &= 0x7F;		//��ʱ��ʱ��12Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TL0 = ((65535-(int)time)/len)%256;		//���ö�ʱ��ֵ
	TH0 = ((65535-(int)time)/len)/256;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
}

void Delay100us()		//@11.0592MHz
{
	unsigned char i, j;

	_nop_();
	_nop_();
	i = 2;
	j = 15;
	do
	{
		while (--j);
	} while (--i);
}

void DAC_Write(uchar DAT)
{
    IIC_Start();                  //I2C������ʼ��ַ
    IIC_SendByte(0x90);				//����PCF8591��ַ�Ӷ�д����λ0��д��
    IIC_WaitAck();
	
    IIC_SendByte(0x40);     //���Ϳ����ֽ�DAC���ʹ�� 
    IIC_WaitAck();              //����Ƿ��ͳɹ�(Ӧ��)
	
    IIC_SendByte(DAT);           //��������������PCF8591תΪģ����AOUT�����
    IIC_WaitAck();
	
    IIC_Stop();                   //I2Cֹͣ�ź�
}

void make_sin()	//���Ҳ��������ã�����Ƶ�������
{
	int i=0;
	for(i;i<len;i++)
	{
		sin_tab[i]=range*sin_tab[i];
		sin_tab[i]=(uchar)sin_tab[i];	//ǿ������ת������PCF8591��ȡ
	}
	Timer0Init();
}

void ser_timer()interrupt 1 //�жϷ�������DAC������Ҳ�
{
	if(flag==len+1)
	{
		flag=0;
	}
	DAC_Write(sin_tab[flag]);
	flag++;
}

void Scan_Keys()	//����ɨ�裬ѭ�����ò���������ȣ�S4�޸Ĳ����ͣ�S5�޸ķ��ȣ�
{
	if(S4==0)
	{
		Delay100us();
		if(S4==0)
		{
			if(sin_status==7)
			{
				sin_status=1;
			}
			sin_status++;
		}
	}	
	
	if(S5==0)
	{
		Delay100us();
		if(S5==0)
		{
			if(sin_range==1)
			{
				sin_range=0.1;
			}
			sin_range=sin_range+0.1;
		}
	}
}

void set_para()	//���ݰ������ò�����
{
	Scan_Keys();
	switch(sin_status){
		case 1:
			freq=261.6;
			break;
		case 2:
			freq=293.6;
			break;
		case 3:
			freq=329.6;
			break;
		case 4:
			freq=349.2;
			break;
		case 5:
			freq=392;
			break;
		case 6:
			freq=440;
			break;
		case 7:
			freq=493.8;
			break;
	}
	range=sin_range;
}

int main()
{
	init_sys();
	while(1)
	{
		set_para();
		make_sin();
	}
}