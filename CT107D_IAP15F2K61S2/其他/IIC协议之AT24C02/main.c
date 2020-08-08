#include<stc15f2k60s2.h>
#include"iic.h"
typedef unsigned char uchar;
typedef unsigned int uint;
uchar code dig_code[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};

void delay(uint t)
{
	while(t--);
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

void display(uchar pos,uchar num)
{
	delay(500);
	P0=0xff;
	select(6);
	P0=0x01<<pos;
	select(7);
	P0=dig_code[num];
}

void write_eeprom(uchar addr,uchar dat)
{
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck(); //发送控制字节
	IIC_SendByte(addr);
	IIC_WaitAck(); //发送需要写入的地址
	IIC_SendByte(dat);
	IIC_WaitAck(); //发送需要写入的数据
	IIC_Stop();
}

uchar read_eeprom(uchar addr)
{
	uchar dat;
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(addr);
	IIC_WaitAck();
	IIC_Stop(); //伪写入操作
	
	IIC_Start();
	IIC_SendByte(0xa1);
	IIC_WaitAck();
	dat=IIC_RecByte();
	IIC_SendAck(1); //发送非响应标志，停止接收数据
	IIC_Stop();
	return dat;
}

void main()
{
	uchar temp;
	init_sys();
	write_eeprom(0xff,0x05);
	delay(1000);
	temp=read_eeprom(0xff);
	while(1)
	{
		display(0,temp);
		delay(500);
	}
}