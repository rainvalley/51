#include<stc15f2k60s2.h>
typedef unsigned char uchar;
typedef unsigned int uint;
uchar code dig_code[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x86};
sbit IR = P1^1;
sbit L1 = P0^0;
uint irdata[4]={0,0,0,0};
void delay(uint t)
{
	while(t--);
}

void delay140us()		//@11.0592MHz
{
	unsigned char i, j;

	i = 2;
	j = 126;
	do
	{
		while (--j);
	} while (--i);
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
	select(5);
	P0=0x00;
	select(4);
	P0=0xff;
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

void init_int0()
{
	IR=1;
	EX0=1;
	IT0=1;
	EA=1;
}

void display_ir()
{
	display(0,irdata[0]/100);
	display(1,irdata[0]%100/10);
	display(2,irdata[0]%10);
	
	display(5,irdata[2]/100);
	display(6,irdata[2]%100/10);
	display(7,irdata[2]%10);
}

//�����յ������ź�ʱ��IR���õ͵�ƽ�������ж�
void ser_int0() interrupt 1
{
	uchar i,j,n=0;
	delay(2000);
	if(IR==1) //�ж��Ƿ�Ϊ����
	{
		return;
	}
	while(!IR); //�������ز�9ms���͵�ƽ
	while(IR);	//���������4.5ms���ߵ�ƽ
	
	for(j=0;j<4;j++)
	{
		for(i=0;i<4;i++)
		{
			while(!IR); //����0/1��ӵ����ͬʱ����ز����͵�ƽ			
			while(IR)
			{
				delay140us(); //ÿ140us������ֵ��һ��ͳ�ƿ���ʱ��
				n++;
				if(n>=30)
				{
					return;
				}
			}
			irdata[j]=irdata[j]>>1;
			if(n>=8) //������ʱ�����1120usʱ���ж�����Ϊ1
			{
				irdata[j]|=0x80;
			}
			n=0;
		}
	}
}

void main()
{
	init_sys();
	init_int0();
	while(1)
	{
		display_ir();
	}
}