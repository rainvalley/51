#include<reg52.h>
#include"onewire.h"
#include"ds1302.h"
#include"data.h"
#include"iic.h"
#include"intrins.h"
void Display_Dynamic();
uchar code dig_code[10]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};//�������ʾ����
uchar time[7]={0x00,0x00,0x00,0x01,0x01,0x01,0x20};//DS1302��ʼ������
uint temp=0;
int data_pcf8591=0;
uint data_at24c02=0;
uint distance=0;
sbit TX=P1^0;
sbit RX=P1^1;
uint j;
//��ʱ����
void Delay(uint t)
{
	while(t--);
}

//ѡ��138�����������
void selectHC573(uchar channel)
{
	switch(channel)
	{
		case 4:
		P2=(P2&0x1f)|0x80;//���ı�P2�����˿ڵ�ƽ������¸ı�138���������룬����ǰ��һλ��channel*2��
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
	}
}

//��λ���������
void DisplaySMG_Bit(uchar value,uchar pos)
{	
	selectHC573(6);
	P0=0X01<<pos;//channelѡ��Ϊ6ʱѡ�������λ��
	selectHC573(7);
	P0=dig_code[value];//channelѡ��Ϊ7ʱѡ���������ʾ����
}

//��ȡ�¶�
void tempget()
{
	uchar low,high;
	init_ds18b20();
	Write_DS18B20(0XCC);
	Write_DS18B20(0X44);

	init_ds18b20();
	Write_DS18B20(0XCC);
	Write_DS18B20(0Xbe);
/*
DS18B20��������(P11)��
1-��ʼ��
2-����ROM
3-ת���¶�(0x44)/��ʼ��ȡ�¶�Scratchpad(0xbe)
*/

	low=Read_DS18B20();
	high=Read_DS18B20();

	temp=high<<4;
	temp|=(low>>4);//��DS18B20����ת��Ϊʮ���Ʋ���ȡ��������
} 

//��̬��ʾ�¶�
void Display_Dynmaic_temp()
{
	tempget();
	DisplaySMG_Bit(temp%100/10,5);
	Delay(500);
	P0=0XFF;
	DisplaySMG_Bit(temp%10,6);
	Delay(500);
	P0=0XFF;
	//��ʾ�¶���������
	selectHC573(6);
	P0=0X01<<7;
	selectHC573(7);
	P0=0XC6;//��ʾC�������϶�
}
									  
//DS1302��ʼ��

void ds1302_init()
{
	uchar add=0x80;//д���ݵ�ַ
	uchar i;
	Write_Ds1302_Byte(0x8e,0x00); //�ر�д����
	for(i=0;i<7;i++)
	{
		Write_Ds1302_Byte(add,time[i]);
		add=add+2;
	}
	Write_Ds1302_Byte(0x8e,0x80); //����д����
}

//��ȡDS1302ʱ�䣬��ַ�ɼ�P9��WPλΪд��������λ
void ds1302_read()
{
	uchar i;
	uchar add=0x81;//�����ݵ�ַ
	Write_Ds1302_Byte(0x8e,0x00);//�ر�д����
	for(i=0;i<7;i++)
	{
		time[i]=Read_Ds1302_Byte(add);
		add=add+2;
	}
	Write_Ds1302_Byte(0x8e,0x80);//����д����
}

//��̬��ʾʱ��
void Display_Dynmaic_time()
{
	ds1302_read();
	DisplaySMG_Bit(time[1]/16,0); //BCD��ȡʮλ
	Delay(500);
	P0=0XFF;
	DisplaySMG_Bit(time[1]&0x0f,1);//BCD��ȡ��λ
	Delay(500);
	P0=0XFF; //�����Ƿ��ӵ���ʾ
	selectHC573(6);
	P0=0X01<<2;
	selectHC573(7);
	P0=~0X40;
	Delay(500);
	P0=0XFF; //�����Ƿָ�����ʾ
	DisplaySMG_Bit(time[0]/16,3);
	Delay(500);
	P0=0XFF;
	DisplaySMG_Bit(time[0]&0x0f,4);	//�����������ʾ
}

//��A/Dͨ��3��ȡ����
void Read_AIN3()
{
	IIC_Start();		//IIC������ʼ�ź�							
	IIC_SendByte(0x90); 	//PCF8591��д�豸��ַ		
	IIC_WaitAck();  	//�ȴ��ӻ�Ӧ��
			
	IIC_SendByte(0x03); 	//д��PCF8591�Ŀ����ֽ�		
	IIC_WaitAck();  	//�ȴ��ӻ�Ӧ��						
	IIC_Stop(); 		//IIC����ֹͣ�ź�					
	


	IIC_Start();											
	IIC_SendByte(0x91); 		
	IIC_WaitAck();
	 				
	data_pcf8591 = IIC_RecByte();			
	IIC_SendAck(0); 		//������Ӧ���ź�				
	IIC_Stop(); 		//IIC����ֹͣ�ź�					
}


void Display_Dynmaic_pcf8591()
{
	Read_AIN3();
   	DisplaySMG_Bit(data_pcf8591/100,0);
	Delay(500);
	P0=0XFF;
	DisplaySMG_Bit((data_pcf8591%100)/10,1);
	Delay(500);
	P0=0XFF;
	DisplaySMG_Bit(data_pcf8591%10,2);
	Delay(500);
}

void write_at24c02(uchar addr,uint data_at24c02)
{
	IIC_Start();
	IIC_SendByte(0XA0);
	IIC_WaitAck();
	IIC_SendByte(addr);
	IIC_WaitAck();
	IIC_SendByte(data_at24c02);
	IIC_WaitAck();
	IIC_Stop();
}

uint read_at24c02(uchar addr)
{
	uchar tmp_at24c02;
	IIC_Start();
	IIC_SendByte(0XA0);
	IIC_WaitAck();
	IIC_SendByte(addr);
	IIC_WaitAck();

	IIC_Start();
	IIC_SendByte(0XA1);
	IIC_WaitAck();
	tmp_at24c02=IIC_RecByte();
	IIC_SendAck(0);
	IIC_Stop();
	return tmp_at24c02;
}

void Display_Dynmaic_at24c02()
{	
	write_at24c02(0X01,0x0f);
	data_at24c02=read_at24c02(0x01);
	DisplaySMG_Bit(data_at24c02/100,0);
	Delay(500);
	DisplaySMG_Bit(data_at24c02%100/10,1);
	Delay(500);
	DisplaySMG_Bit(data_at24c02%10,2);
	Delay(500);
	
}

void Delay12us()		//@12.000MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 33;
	while (--i);
}

void send_wave()
{
	uchar i;
	for(i=0;i<8;i++)
	{
		TX=1;
		Delay12us();
		TX=0;
		Delay12us();
	}
}

void measure_distance()
{
	uint time_sonic=0;
	TMOD&=0X0F;
	TL1=0X00;
	TH1=0X00;

	send_wave();
	TR1=1;
	while((RX==1)&&(TF1==0));
	TR1=0;

	if(TF1==0)
	{
		time_sonic=TH1;
		time_sonic=(time_sonic<<8)|TL1;
		distance=((time_sonic/10)*17)/100+3;
	}
	else
	{
		TF1=0;
		distance=999;
	}
}

void Display_Dynmaic_sonic()
{
	measure_distance();
	for(j=0;j<100;j++)
	{
		DisplaySMG_Bit(distance/100,0);
		Delay(500);
		DisplaySMG_Bit(distance%100/10,1);
		Delay(500);
		DisplaySMG_Bit(distance%10,2);
		Delay(500);
	}
	
}
int main()
{
	selectHC573(5);
	P0=0X00;//��ʼ��������Դ���رշ�������̵���
	ds1302_init();
	while(1)
	{	
		Display_Dynmaic_temp();
		//Display_Dynmaic_time();
		//Display_Dynmaic_pcf8591();
		//Display_Dynmaic_at24c02();
		//Display_Dynmaic_sonic();
	}
}