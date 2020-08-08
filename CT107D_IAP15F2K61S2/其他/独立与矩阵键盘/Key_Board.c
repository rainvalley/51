#include<stc15f2k60s2.h>
typedef unsigned int uint;
typedef unsigned char uchar;
uchar code dig_code[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x80,0xc6,0xc0,0x86,0x8e,0xbf,0x7f,0xff};
sbit KEY_IN_1 = P4^4;
sbit KEY_IN_2 = P4^2;
sbit KEY_IN_3 = P3^5;
sbit KEY_IN_4 = P3^4;
sbit KEY_OUT_1 = P3^0;
sbit KEY_OUT_2 = P3^1;
sbit KEY_OUT_3 = P3^2;
sbit KEY_OUT_4 = P3^3;
uchar KeySta[4][4]={{1,1,1,1},{1,1,1,1},{1,1,1,1},{1,1,1,1}};
void delay(uint t)
{
	while(t--);
}

void select(uchar channel)
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

void display(uchar pos,uchar dig)
{
	delay(2000);
	P0=0XFF;
	select(6);
	P0=0X01<<pos;
	select(7);
	P0=dig_code[dig];
}

void InitSys()
{
	select(5);
	P0=0X00;
	select(4);
	P0=0XFF;
}

void Timer0Init(void)		//1����@11.0592MHz
{
	AUXR &= 0x7F;		//��ʱ��ʱ��12Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TL0 = 0x66;		//���ö�ʱ��ֵ
	TH0 = 0xFC;		//���ö�ʱ��ֵ���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
	ET0 = 1;
	EA = 1;
}


void Scan_Key_16(void)
{
	uchar i;
	static uchar keyout = 0;		//���󰴼�ɨ�����������
	static uchar keybuff[4][4] = {{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff}};	//���󰴼�ɨ�軺����
	
	keybuff[keyout][0] = (keybuff[keyout][0] << 1) | KEY_IN_1;		//��ÿһ�е�4������ֵ���뻺����
	keybuff[keyout][1] = (keybuff[keyout][1] << 1) | KEY_IN_2;
	keybuff[keyout][2] = (keybuff[keyout][2] << 1) | KEY_IN_3;
	keybuff[keyout][3] = (keybuff[keyout][3] << 1) | KEY_IN_4;
		
	//��������°���״̬
	for(i = 0;i < 4;i ++)
	{
		if((keybuff[keyout][i] & 0x0f) == 0x00)
			KeySta[keyout][i] = 0;		//����4��ɨ��ֵ����0����4��4ms�ڶ��ǰ���״̬����Ϊ������ƽ�Ȱ���
		else if((keybuff[keyout][i] & 0x0f) == 0x0f)
			KeySta[keyout][i] = 1;		//����4��ɨ��ֵ����1����4��4ms�ڶ����ɿ�״̬����Ϊ�������ȶ�����
	}
	
	//ִ����һ�ε�ɨ�����
	keyout ++;
	keyout = keyout & 0x03;		//�����ӵ�4�͹���
	switch(keyout)						//�����������ͷŵ�ǰ������ţ������´ε��������
	{
		case 0:KEY_OUT_4 = 1;KEY_OUT_1 = 0;break;
		case 1:KEY_OUT_1 = 1;KEY_OUT_2 = 0;break;
		case 2:KEY_OUT_2 = 1;KEY_OUT_3 = 0;break;
		case 3:KEY_OUT_3 = 1;KEY_OUT_4 = 0;break;
		default:break;
	}

}

void Ser_Timer0() interrupt 1
{
	Scan_Key_16();
}

int main()
{
	InitSys();
	P3 = 0XFF;
	Timer0Init();
	while(1)
	{
		display(0,KeySta[0][0]);
		display(1,KeySta[1][1]);
		display(2,KeySta[2][2]);
		display(3,KeySta[3][3]);
	}
}