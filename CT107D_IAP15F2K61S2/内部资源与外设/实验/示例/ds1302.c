/*
  ����˵��: DS1302��������
  �������: Keil uVision 4.10 
  Ӳ������: CT107��Ƭ���ۺ�ʵѵƽ̨ 8051��12MHz
  ��    ��: 2011-8-9
  �޸�ʱ�䣺2020-1-19
  �޸Ļ�����IAP15F2K61S2
*/

#include <reg52.h>
#include <intrins.h>

sbit SCK = P1 ^ 7; //ʱ����
sbit SDA = P2 ^ 3; //������
sbit RST = P1 ^ 3; //DS1302ʹ������
void Write_Ds1302(unsigned char temp) //DS1302д����
{
	unsigned char i;
	for (i = 0; i < 8; i++)
	{
		SCK = 0; //����SCK׼��ʵ��������
		SDA = temp & 0x01; //ȡtemp�����λ��ÿ�η���һλ
		temp >>= 1; //��temp���ƣ�����ոշ��͵�һλ
		SCK = 1; //������׼���ã���������ʱ��SDA������д��DS1302
	}
}

void Write_Ds1302_Byte(unsigned char address, unsigned char dat) //���ض���ַд������
{
	RST = 0; 
	_nop_();
	SCK = 0;
	_nop_();
	RST = 1; 
	_nop_(); //SCK�͵�ƽʱ����RST��ƽ
	Write_Ds1302(address); //д���ַ
	Write_Ds1302(dat); //д������
	RST = 0; //����RST��ƽ��λ
}

unsigned char Read_Ds1302_Byte(unsigned char address)
{
	unsigned char i, temp = 0x00;
	RST = 0;
	_nop_();
	SCK = 0;
	_nop_();
	RST = 1;
	_nop_(); //SCK�͵�ƽʱ����RST��ƽ
	Write_Ds1302(address);
	for (i = 0; i < 8; i++)
	{
		SCK = 0; //����SCK�����ݱ�д��SDA�϶�ȡ
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
