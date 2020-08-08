#include<stc15f2k60s2.h>
#include"iic.h"
typedef unsigned char uchar;
typedef unsigned int uint;
uchar code dig_code[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
void select(uchar channel)
{
	switch(channel)
	{
		case 4:
			P2=(P2&0x1F)|0x80;
			break;
		case 5:
			P2=(P2&0x1F)|0xa0;
			break;
		case 6:
			P2=(P2&0x1F)|0xc0;
			break;
		case 7:
			P2=(P2&0x1F)|0xe0;
			break;
		default:
			P2=(P2&0x1F);
			break;
	}
}

void init_sys()
{
	select(4);
	P0=0xFF;
	select(5);
	P0=0x00;
	select(0);
}

void delay(uint t)
{
	while(t--);
}

void display(uchar pos,uchar num)
{
	delay(1000);
	P0=0xFF;
	select(6);
	P0=0x01<<pos;
	select(7);
	P0=dig_code[num];
}

uchar ADC_AIN(uchar ain)
{
	uchar volta;
	IIC_Start();
	IIC_SendByte(0x90); //给PCF8591发送写控制字
	IIC_WaitAck();
	IIC_SendByte(ain+0x40); //给写入通道数，并允许模拟量输出
	IIC_WaitAck();
	IIC_Stop();
	
//读写方向发生变化，重新开始通信
	
	IIC_Start();
	IIC_SendByte(0x91); //给PCF8591发送读控制字
	IIC_WaitAck();
	volta=IIC_RecByte();
	IIC_SendAck(1);
	IIC_Stop();
	
	return volta;
}

void DAC(float volta)
{
	float dat = (volta/5.0)*255;
	IIC_Start();
	IIC_SendByte(0x90); //发送写控制字
	IIC_WaitAck();
	IIC_SendByte(0x40); //允许模拟量输出
	IIC_WaitAck();
	IIC_SendByte(dat); //发送需要转换的数字量
	IIC_WaitAck();
	IIC_Stop();
}

void display_ADC()
{
	uchar volta1,volta3;
	volta1=ADC_AIN(1);
	display(0,volta1/100);
	display(1,(volta1%100)/10);
	display(2,volta1%10);
	volta3=ADC_AIN(3);
	display(5,volta3/100);
	display(6,(volta3%100)/10);
	display(7,volta3%10);
}

void main()
{
	
	init_sys();
	DAC(2.45);
	while(1)
	{
		display_ADC();
	}
}