#include<stc15f2k60s2.h>
typedef unsigned char uchar;
typedef unsigned int uint;
uchar code dig_code[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x80,0xc6,0xc0,0x86,0x8e,0xbf,0x7f,0xff};
sbit S7 = P3^0;
uint count = 0;
void Show_Count(uint count);

void Select(uchar channel)
{
	switch(channel)
	{
		case 4: 
			P2=(P2&0X1f)|0X80;//���ı�P2����IO�ڵ�ƽ������¸ı�138����������
			break;
		case 5: 
			P2=(P2&0X1f)|0Xa0;
			break;
		case 6: 
			P2=(P2&0X1f)|0Xc0;
			break;
		case 7: 
			P2=(P2&0X1f)|0Xe0;
			break;
	}
}

void Delay(uint t)
{
	while(t--);
}

void Init_Sys()
{
	Select(5);
	P0=0x00;
	Select(4);
	P0=0XFF;
}

void Scan_Key()
{
	if(S7==0)
	{
		count++;
		while(S7==0)
		{
			Show_Count(count);
		}
	}
}

void Timer0Init(void)		//50����@11.0592MHz
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

void Display(uchar pos,uchar dig)
{
	Delay(200);
	P0=0XFF;
	Select(6);
	P0=0X01<<pos;
	Select(7);
	P0=dig_code[dig];
}

void Show_Count(uint count)
{
	Display(7,count%10);
	Display(6,count/10%10);
	Display(5,count/100);
}

void Ser_Timer0() interrupt 1
{
	Scan_Key();
}

void main()
{
	Init_Sys();
	Timer0Init();
	while(1)
	{
		Show_Count(count);
	}
}