#include<stc15f2k60s2.h>
#include "iic.h"
#include "ds1302.h"
typedef unsigned char uchar;
typedef unsigned int uint;
uchar code dig_code[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf,0xff,0xc6};
//定义变量：触发、长按、工作模式、设置的湿度阈值、当前湿度
uchar trg,cont,mode=0,limit=50,hum;
//定义标志量：自动模式下湿度阈值标志、蜂鸣器标志、灌溉标志、手动模式下达到对应湿度的标志符
uchar flag_set,flag_beep,flag_water,flag_on;
uchar buff_tube[8]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
uchar time[3]={0x00,0x30,0x08};
sbit L1=P0^0;
sbit L2=P0^1;
sbit beep=P0^6;
sbit relay=P0^4;
void save_config();
void init_ds1302();
void read_config();

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
	select(5);
	P0=0x00;
	select(4);
	P0=0xff;
	select(0);
	init_ds1302();
	read_config();
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

void scan_key()
{
	uchar dat=P3^0xff;
	trg=dat&(dat^cont);
	cont=dat;
}

void fun_key()
{
	if(trg==0x01)
	{
		if(mode==0) //自动模式
		{
			mode=1;
		}
		else if(mode==1) //手动模式
		{
			mode=0;
		}
	}
	if(trg==0x02)
	{
		if(mode==0&&flag_set==0)
		{
			flag_set=1;
		}
		else if(mode==0&&flag_set==1)
		{
			save_config();
			flag_set=0;
		}
		else if(mode==1)
		{
			flag_beep=!flag_beep;
		}
	}
	if(trg==0x04)
	{
		if(mode==0&&flag_set==1)
		{
			limit++;
			if(limit>=100)
			{
				limit=99;
			}
		}
		else if(mode==1)
		{
			flag_water=1;
		}
	}
	if(trg==0x08)
	{
		if(mode==0&&flag_set==1)
		{
			if(limit<=0)
			{
				limit=1;
			}
			limit--;
		}
		else if(mode==1)
		{
			flag_water=0;
		}
	}
}

void save_config()
{
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(0x00);
	IIC_WaitAck();
	IIC_SendByte(limit);
	IIC_WaitAck();
	IIC_Stop();
}

void read_config()
{
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(0x00);
	IIC_WaitAck();
	IIC_Stop();
	
	IIC_Start();
	IIC_SendByte(0xa1);
	IIC_WaitAck();
	limit=IIC_RecByte();
	IIC_Ack(0);
	IIC_Stop();
}

uchar read_vlota()
{
	uchar dat;
	IIC_Start();
	IIC_SendByte(0x90);
	IIC_WaitAck();
	IIC_SendByte(3);
	IIC_WaitAck();
	IIC_Stop();
	
	IIC_Start();
	IIC_SendByte(0x91);
	IIC_WaitAck();
	dat=IIC_RecByte();
	IIC_Ack(0);
	IIC_Stop();
	
	return dat;
}

void init_ds1302()
{
	Write_Ds1302(0x8e,0x00);
	Write_Ds1302(0x80,time[0]);
	Write_Ds1302(0x82,time[1]);
	Write_Ds1302(0x84,time[2]);
	Write_Ds1302(0x8e,0x80);
}

void read_ds1302()
{
	time[1]=Read_Ds1302(0x83);
	time[2]=Read_Ds1302(0x85);
}

void get_hum()
{
	uchar volta=read_vlota();
	hum=(1.0*volta/255.0)*100-1;
	if(hum>=255)
	{
		hum=0;
	}
}

void set_display()
{
	if(mode==1||(mode==0&&flag_set==0))
	{
		buff_tube[0]=dig_code[time[2]/10];
		buff_tube[1]=dig_code[time[2]%10];
		buff_tube[2]=dig_code[10];
		buff_tube[3]=dig_code[time[1]/16];
		buff_tube[4]=dig_code[time[1]%16];
		buff_tube[5]=dig_code[10];
		buff_tube[6]=dig_code[hum/10];
		buff_tube[7]=dig_code[hum%10];
	}
	else if(mode==0&&flag_set==1)
	{
		buff_tube[0]=dig_code[10];
		buff_tube[1]=dig_code[10];
		buff_tube[2]=dig_code[11];
		buff_tube[3]=dig_code[11];
		buff_tube[4]=dig_code[11];
		buff_tube[5]=dig_code[11];
		buff_tube[6]=dig_code[limit/10];
		buff_tube[7]=dig_code[limit%10];
	}
}

void set_led(uchar dat)
{
	P0=0xff;
	select(4);
	P0=dat;
	select(0);
}

void set_beep(uchar dat)
{
	P0=0x00;
	select(5);
	P0=dat;
	select(0);
}

void set_others()
{
	if(mode==0)
	{
		set_led(0xfe);
	}
	else if(mode==1)
	{
		set_led(0xfd);
	}
	
	if(mode==0) //自动模式下
	{
		if(flag_water)
		{
			set_beep(0x10);
		}
		else
		{
			set_beep(0x00);
		}
	}
	else if(mode==1) //手动模式下
	{
		if(flag_on) //达到了湿度阈值
		{
			if(flag_beep==1&&flag_water==1) //蜂鸣器&灌溉同时打开
			{
				set_beep(0x50);
			}
			else if(flag_beep==1&&flag_water==0) //蜂鸣器打开&灌溉关闭
			{
				set_beep(0x40);
			}
			else if(flag_beep==0&&flag_water==1) //蜂鸣器关闭&灌溉打开
			{
				set_beep(0x10);
			}
			else //蜂鸣器&灌溉均关闭
			{
				set_beep(0x00);
			}
		}
		else //在湿度阈值以下
		{
			if(flag_water) //手动打开灌溉
			{
				set_beep(0x10);
			}
			else
			{
				set_beep(0x00);
			}
		}
	}
}

void check_hum()
{
	if(mode==0) //自动模式下判定flag_water
	{
		if(hum<limit)
		{
			flag_water=1;
		}
		else
		{
			flag_water=0;
		}
	}	
	if(mode==1) //手动模式下判定flag_on
	{
		if(hum<limit)
		{
			flag_on=1;
		}
		else
		{
			flag_on=0;
		}
	}
}

void init_timer0(void)		//1毫秒@11.0592MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0xCD;		//设置定时初值
	TH0 = 0xD4;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;
	EA = 1;
}

void ser_timer0() interrupt 1
{
	static uchar count_50ms,count_2ms;
	count_50ms++;
	count_2ms++;
	if(count_2ms>=2)
	{
		count_2ms=0;
		set_display();
		display();
	}
	if(count_50ms>=50)
	{
		count_50ms=0;
		read_ds1302();
		scan_key();
		fun_key();
	}
}

void main()
{
	init_sys();
	init_timer0();
	while(1)
	{
		get_hum();
		check_hum();
		set_others();
	}
}