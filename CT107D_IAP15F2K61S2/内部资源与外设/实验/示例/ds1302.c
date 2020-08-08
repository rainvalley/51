/*
  程序说明: DS1302驱动程序
  软件环境: Keil uVision 4.10 
  硬件环境: CT107单片机综合实训平台 8051，12MHz
  日    期: 2011-8-9
  修改时间：2020-1-19
  修改环境：IAP15F2K61S2
*/

#include <reg52.h>
#include <intrins.h>

sbit SCK = P1 ^ 7; //时钟线
sbit SDA = P2 ^ 3; //数据线
sbit RST = P1 ^ 3; //DS1302使能引脚
void Write_Ds1302(unsigned char temp) //DS1302写函数
{
	unsigned char i;
	for (i = 0; i < 8; i++)
	{
		SCK = 0; //拉低SCK准备实现上升沿
		SDA = temp & 0x01; //取temp的最低位，每次发送一位
		temp >>= 1; //将temp右移，清除刚刚发送的一位
		SCK = 1; //将数据准备好，在上升沿时，SDA的数据写入DS1302
	}
}

void Write_Ds1302_Byte(unsigned char address, unsigned char dat) //向特定地址写入数据
{
	RST = 0; 
	_nop_();
	SCK = 0;
	_nop_();
	RST = 1; 
	_nop_(); //SCK低电平时拉高RST电平
	Write_Ds1302(address); //写入地址
	Write_Ds1302(dat); //写入数据
	RST = 0; //拉低RST电平复位
}

unsigned char Read_Ds1302_Byte(unsigned char address)
{
	unsigned char i, temp = 0x00;
	RST = 0;
	_nop_();
	SCK = 0;
	_nop_();
	RST = 1;
	_nop_(); //SCK低电平时拉到RST电平
	Write_Ds1302(address);
	for (i = 0; i < 8; i++)
	{
		SCK = 0; //拉低SCK后，数据被写到SDA上读取
		temp >>= 1;
		if (SDA)
			temp |= 0x80;
		SCK = 1; 
	}
	RST = 0;
	_nop_();
	SCK = 0;
	_nop_();
	SCK = 1;
	_nop_();
	SDA = 0;
	_nop_();
	SDA = 1;
	_nop_();
	return (temp);
}
