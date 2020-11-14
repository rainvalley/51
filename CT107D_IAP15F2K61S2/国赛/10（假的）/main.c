#include <stc15f2k60s2.h>
#include <string.h>
#include <stdio.h>
#include "ds1302.h"
#include "onewire.h"
#include "iic.h"
sbit KEY_IN_0=P4^4;
sbit KEY_IN_1=P4^2;
sbit KEY_IN_2=P3^5;
sbit KEY_IN_3=P3^4;
sbit KEY_OUT_0=P3^0;
sbit KEY_OUT_1=P3^1;
sbit KEY_OUT_2=P3^2;
sbit KEY_OUT_3=P3^3;
typedef unsigned char uchar;
typedef unsigned int uint;
uchar dig_code[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
uchar buff_tube[]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
uchar key_back[4][4]={{1,1,1,1},{1,1,1,1},{1,1,1,1},{1,1,1,1}};
uchar key_stat[4][4]={{1,1,1,1},{1,1,1,1},{1,1,1,1},{1,1,1,1}};
uchar time[]={50,59,23};
uchar str[10];
uchar index=0;
uchar dis=20,p_dis=35,p_temp=30;
float volta,temp=24.23;
uchar trg,cont;

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

void init_uart(void)		//9600bps@12.000MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x01;		//串口1选择定时器2为波特率发生器
	AUXR |= 0x04;		//定时器2时钟为Fosc,即1T
	T2L = 0xC7;		//设定定时初值
	T2H = 0xFE;		//设定定时初值
	AUXR |= 0x10;		//启动定时器2
	EA = 1;
	ES = 1;
}

void send_data(uchar *str)
{
	while(*str!='\0')
	{
		SBUF=*str++;
		while(TI==0);
		TI=0;
	}
}

void send_uart()
{
	uchar buff[10];
	if(strncmp(str,"ST",2)==0)
	{
		sprintf(buff,"$%u,%.2f\r\n",(uint)dis,temp);
		send_data(buff);
	}
	else if(strncmp(str,"PARA",4)==0)
	{
		sprintf(buff,"$%u,%u\r\n",(uint)p_dis,(uint)p_temp);
		send_data(buff);
	}
	else
	{
		send_data("ERROR\r\n");
	}
}

void ser_uart() interrupt 4
{
	uchar temp;
	if(RI)
	{
		RI=0;
		temp=SBUF;
		if(temp!='\n')
		{
			str[index]=temp;
			index++;
		}
		else
		{
			index=0;
			send_uart();
		}
	}
}

void get_temp()
{
	uchar LSB,MSB;
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0x44);
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0xbe);
	LSB=Read_DS18B20();
	MSB=Read_DS18B20();
	init_ds18b20();
	temp=(MSB<<8|LSB)*0.0625;
}

void dac(float volta)
{
	float dat=(volta/5.0)*255;
	IIC_Start();
	IIC_SendByte(0x90);
	IIC_WaitAck();
	IIC_SendByte(0x40);
	IIC_WaitAck();
	IIC_SendByte(dat);
	IIC_WaitAck();
	IIC_Stop();
}

void adc()
{
	IIC_Start();
	IIC_SendByte(0x90);
	IIC_WaitAck();
	IIC_SendByte(0x01);
	IIC_WaitAck();
	IIC_Stop();
	
	IIC_Start();
	IIC_SendByte(0x91);
	volta=IIC_RecByte()/255.0*500;
	IIC_Ack(0);
	IIC_Stop();
}

void save_data(uchar add,uchar dat)
{
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(add);
	IIC_WaitAck();
	IIC_SendByte(dat);
	IIC_WaitAck();
	IIC_Stop();
}

uchar read_data(uchar add)
{
	uchar dat;
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(add);
	IIC_WaitAck();
	IIC_Stop();
	
	IIC_Start();
	IIC_SendByte(0xa1);
	dat=IIC_RecByte();
	IIC_Ack(0);
	IIC_Stop();
	return dat;
}

uchar bcd_to_num(uchar bcd)
{
	uchar a,b;
	a=bcd/16;
	b=bcd%16;
	return 10*a+b;
}

uchar num_to_bcd(uchar num)
{
	uchar a,b;
	a=num/10;
	b=num%10;
	return 16*a+b;
}

void write_time()
{
	uchar i,add=0x80;
	Write_Ds1302(0x8e,0x80);
	for(i=0;i<3;i++)
	{
		Write_Ds1302(add,num_to_bcd(time[i]));
		add=add+2;
	}
	Write_Ds1302(0x8e,0x00);
}

void read_time()
{
	uchar i,add=0x81;
	for(i=0;i<3;i++)
	{
		time[i]=bcd_to_num(Read_Ds1302(add));
		add=add+2;
	}
}

void scan_key()
{
	uchar dat=P3^0xff;
	trg=dat&(dat^cont);
	cont=dat;
}

void display()
{
	static uchar index=0;
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

void set_display()
{
	buff_tube[0]=dig_code[0];
	buff_tube[1]=dig_code[1];
	buff_tube[2]=dig_code[2];
	buff_tube[3]=dig_code[3];
	buff_tube[4]=dig_code[4];
	buff_tube[5]=dig_code[5];
	buff_tube[6]=dig_code[6];
	buff_tube[7]=dig_code[7];
}

void scan_keys()
{
	uchar i;
	static uchar keyout=0;
	static uchar key_buff[4][4]={{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff}};
	key_buff[keyout][0]=key_buff[keyout][0]<<1|KEY_OUT_0;	
	key_buff[keyout][1]=key_buff[keyout][1]<<1|KEY_OUT_1;	
	key_buff[keyout][2]=key_buff[keyout][2]<<1|KEY_OUT_2;	
	key_buff[keyout][3]=key_buff[keyout][3]<<1|KEY_OUT_3;	
	
	for(i=0;i<4;i++)
	{
		if(key_buff[keyout][i]==0x00)
		{
			key_stat[keyout][i]=0;
		}
		else if(key_buff[keyout][i]==0xff)
		{
			key_stat[keyout][i]=1;
		}
	}
	
	keyout++;
	keyout&=0x03;
	
	switch(keyout)
	{
		case 0:KEY_OUT_3=1;KEY_OUT_0=0;break;
		case 1:KEY_OUT_0=1;KEY_OUT_1=0;break;
		case 2:KEY_OUT_1=1;KEY_OUT_2=0;break;
		case 3:KEY_OUT_2=1;KEY_OUT_3=0;break;
		default:break;
	}
	
}

void key_fun(uchar i,uchar j)
{
	
}

void key_press()
{
	int i,j;
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			if(key_stat[i][j]!=key_back[i][j])
			{
				if(key_stat[i][j]==0)
				{
					key_fun(i,j);
				}
				key_back[i][j]=key_stat[i][j];
			}
		}
	}
}

void init_timer0(void)		//1毫秒@12.000MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x18;		//设置定时初值
	TH0 = 0xFC;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	EA = 1;
	ET0 = 1;
}

void ser_timer0() interrupt 1
{
	scan_key();
	set_display();
	display();
}

int main()
{
	init_sys();
	init_uart();
	init_timer0();
	while(1)
	{
		
	}
}