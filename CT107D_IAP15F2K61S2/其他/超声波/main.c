#include<stc15f2k60s2.h>
#include<intrins.h>
typedef unsigned char uchar;
typedef unsigned int uint;
uchar code dig_code[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x86};
sbit TX=P1^0; //��������
sbit RX=P1^1; //��������
uint distance=0;
void delay(uint t)
{
	while(t--);
}

void delay12us()
{
	unsigned char i;

	_nop_();
	_nop_();
	_nop_();
	i = 33;
	while (--i);
}


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
		case 0:
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

void display(uchar pos,uchar value)
{
	delay(1000);
	P0=0xff;
	select(6);
	P0=0x01<<pos;
	select(7);
	P0=dig_code[value];
}

void send_wave()
{
	uchar i;
	for(i=0;i<8;i++) //����8��40KHz�ĳ�����
	{
		TX=1;
		delay12us();
		TX=0;
		delay12us();
	}
}

void measure_distance()
{
	uint time=0;
	TMOD|=0x10;
	TL1=0;
	TH1=0;
	TF1=0;
	ET1=0;
	
	send_wave(); //��ʼ���Ͳ�������ʱ��ͳ�ƽ��յ�ʱ��
	TR1=1;
	while((RX==1)&&(TF1==0)); //��δ���յ������Ҷ�ʱ��δ���ʱ����ѭ��
	TR1=0;
	
	if(TF1==0) //��ʱ��δ��������ݷ���ʱ��������
	{
		time=(TH1<<8)|TL1;
		distance=((time/10)*17)/100+3;
	}
	else //��ʱ������������������
	{
		TF1=0;
		distance=999;
	}
}

void display_distance()
{
	if(distance==999)
	{
		display(0,10);
	}
	else
	{
		display(0,distance/100);
		display(1,distance%100/10);
		display(2,distance%10);
	}
}

void init_timer0(void)		//50����@11.0592MHz
{
	AUXR &= 0x7F;		//��ʱ��ʱ��12Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TL0 = 0x00;		//���ö�ʱ��ֵ
	TH0 = 0x4C;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
	ET0 = 1;
	EA = 1;
}

void ser_timer0() interrupt 1
{
	static uchar count=0;
	if(count==4)
	{
		count=0;
		measure_distance();
	}
	count++;
}	


int main()
{
	init_sys();
	init_timer0();
	while(1)
	{
		display_distance();
	}
}