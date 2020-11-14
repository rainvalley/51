#include <stc15f2k60s2.h>
#include "iic.h"
typedef unsigned char uchar;
typedef unsigned int uint;
uchar code dig_code[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x8e,0xc1,0xff};
uchar buff_tube[]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
//����������̰���������������ʶ����dac��ʶ����led��ʾ��ʶ�����������ʾ��ʶ��
uchar trg,cont,flag_measure,flag_dac,flag_led=1,flag_smg=1;
uchar flag_20ms,flag_50ms;
//�����������ѹ���˴�ʹ��������ʾ����0-500����NE555Ƶ�ʡ�NE555�Ĳ�������Ƶ��
uint volta,freq,count_freq;

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
		case 6:
			P2=(P2&0x1f)|0xc0;
			break;
		case 7:
			P2=(P2&0x1f)|0xe0;
			break;
		default:
			P2=(P2&0x1f);
			break;
	}
}

void init_sys()
{
	select(4);
	P0=0xff;
	select(5);
	P0=0x00;
	select(0);
}

void display()
{
	static uchar index;
	select(7);
	P0=0xff;
	select(6);
	P0=0x01<<index;
	select(7);
	P0=buff_tube[index];
	select(0);
	index++;
	index&=0x07;
}

void scan_key()
{
	uchar dat=P3^0xff;
	trg=dat&(dat^cont);
	cont=dat;
}

void key_fun()
{
	if(trg==0x01)
	{
		flag_smg=!flag_smg;
	}
	if(trg==0x02)
	{
		flag_led=!flag_led;
	}
	if(trg==0x04)
	{
		flag_dac=~flag_dac;
	}
	if(trg==0x08)
	{
		flag_measure=~flag_measure;
	}
}

void set_display()
{
	if(flag_smg)
	{
		if(flag_measure==0) //��ѹ��ʾ
		{
			buff_tube[0]=dig_code[11];
			buff_tube[1]=dig_code[12];
			buff_tube[2]=dig_code[12];
			buff_tube[3]=dig_code[12];
			buff_tube[4]=dig_code[12];
			buff_tube[5]=dig_code[volta/100]+0x80; //��ԭ�������0x80���ɵĵõ�С����
			buff_tube[6]=dig_code[(volta%100)/10];
			buff_tube[7]=dig_code[volta%10];
		}
		else //Ƶ����ʾ
		{
			buff_tube[0]=dig_code[10];
			buff_tube[1]=dig_code[12];
			buff_tube[2]=dig_code[12];
			buff_tube[3]=dig_code[(freq/10000)%10];
			buff_tube[4]=dig_code[(freq/1000)%10];
			buff_tube[5]=dig_code[(freq/100)%10];
			buff_tube[6]=dig_code[(freq/10)%10];
			buff_tube[7]=dig_code[freq%10];
			if(buff_tube[3]==dig_code[0]) //���������Ϩ��
			{
				buff_tube[3]=0xff;
				if(buff_tube[4]==dig_code[0]) 
				{
					buff_tube[4]=0xff;
					if(buff_tube[5]==dig_code[0])
					{
						buff_tube[5]=0xff;
						if(buff_tube[6]==dig_code[0])
						{
							buff_tube[6]=0xff;
							if(buff_tube[7]==dig_code[0])
							{
								buff_tube[7]=0xff;
							}
						}
					}
				}
			}
		}
	}
	else
	{
		buff_tube[0]=dig_code[12];
		buff_tube[1]=dig_code[12];
		buff_tube[2]=dig_code[12];
		buff_tube[3]=dig_code[12];
		buff_tube[4]=dig_code[12];
		buff_tube[5]=dig_code[12];
		buff_tube[6]=dig_code[12];
		buff_tube[7]=dig_code[12];
	}
}

void adc() //����A-D��ע������ֽ���Ҫ����D-A
{
	IIC_Start();
	IIC_SendByte(0x90);
	IIC_WaitAck();
	IIC_SendByte(0x43);
	IIC_WaitAck();
	IIC_Stop();
	
	IIC_Start();
	IIC_SendByte(0x91);
	IIC_WaitAck();
	volta=IIC_RecByte()*2;
	IIC_Ack(0);
	IIC_Stop();
}

void dac() //����D-A��ֱ��д���ѹ��Ӧ��ֵ
{
	if(flag_dac==0)
	{
		IIC_Start();
		IIC_SendByte(0x90);
		IIC_WaitAck();
		IIC_SendByte(0x40);
		IIC_WaitAck();
		IIC_SendByte(102);
		IIC_WaitAck();
		IIC_Stop();
	}
	else
	{
		IIC_Start();
		IIC_SendByte(0x90);
		IIC_WaitAck();
		IIC_SendByte(0x40);
		IIC_WaitAck();
		IIC_SendByte(volta/2);
		IIC_WaitAck();
		IIC_Stop();
	}
}

void set_led()
{
	P0=0xff;
	select(4);
	if(flag_led)
	{
		if(flag_measure==0) //������ѹ
		{
			P0=0xfe;
			if(volta<150)
			{
				P0|=0x04; //����3λ��Ϊ1
			}
			else if(volta<250)
			{
				P0&=~0x04; //����3λ��Ϊ0
			}
			else if(volta<350)
			{
				P0|=0x04;
			}
			else
			{
				P0&=~0x04;
			}
		}
		else //����NE555Ƶ��
		{
			P0=0xfd;
			if(volta<1000)
			{
				P0|=0x08; //����4λ��Ϊ1
			}
			else if(volta<5000)
			{
				P0&=~0x08; //����4λ��Ϊ0
			}
			else if(volta<10000)
			{
				P0|=0x08;
			}
			else
			{
				P0&=~0x08;
			}
		}
		if(flag_dac==0)
		{
			P0|=0x10;
		}
		else
		{
			P0&=~0x10;
		}
	}
	else
	{
		P0=0xff;
	}
	select(0);
}

void init_timer0(void)
{
	TMOD = 0x06;
	TL0 = TH0 = 0xff;
	ET0 = 1;
	TR0 = 1;
	EA = 1;
}

void ser_timer0()interrupt 1
{
	count_freq++;
}

void init_timer1() //1����@12.000MHz
{
	TL1 = 0x18;		//���ö�ʱ��ֵ
	TH1 = 0xFC;		//���ö�ʱ��ֵ
	TF1 = 0;		//���TF1��־
	TR1 = 1;		//��ʱ��1��ʼ��ʱ
	ET1 = 1;
	EA = 1;
}

void ser_timer1() interrupt 3
{
	static uchar count_20ms,count_2ms,count_50ms;
	static uint count_500ms; //ע�������Χ
	count_20ms++;
	count_2ms++;
	count_50ms++;
	count_500ms++;
	if(count_2ms>=2)
	{
		count_2ms=0;
		set_display();
		display();
		set_led(); //ͬ��ɨ��ʱ���������˸
	}
	if(count_20ms>=20)
	{
		count_20ms=0;
		flag_20ms=~flag_20ms;
	}
	if(count_50ms>=50)
	{
		count_50ms=0;
		flag_50ms=~flag_50ms;
	}
	if(count_500ms>=500)	
	{
		TR0=0;
		count_500ms=0;
		freq=count_freq*2;
		count_freq=0;
		TR0=1;
	}
}

void main()
{
	init_sys();
	init_timer0();
	init_timer1();
	while(1)
	{
		if(flag_20ms)
		{
			scan_key();
			key_fun();
			dac(); //��ADC DAC����8591�����ݻ���
		}
		if(flag_50ms)
		{
			adc();
		}
	}
}