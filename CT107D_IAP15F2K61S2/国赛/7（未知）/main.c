#include <stc15f2k60s2.h>
#include "iic.h"
#include "ds1302.h"
typedef unsigned char uchar;
typedef unsigned int uint;
sbit KEY_IN_0=P4^4;
sbit KEY_IN_1=P4^2;
sbit KEY_IN_2=P3^5;
sbit KEY_IN_3=P3^4;
sbit KEY_OUT_0=P3^0;
sbit KEY_OUT_1=P3^1;
sbit KEY_OUT_2=P3^2;
sbit KEY_OUT_3=P3^3;
uchar dig_code[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf,0xff};
uchar buff_tube[]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
uchar key_back[4][4]={{1,1,1,1},{1,1,1,1},{1,1,1,1},{1,1,1,1}};
uchar key_stat[4][4]={{1,1,1,1},{1,1,1,1},{1,1,1,1},{1,1,1,1}};
char time[]={55,59,23};
uchar dat[]={0x00,0x00,0x00,0x00};
uchar volta,type,flag_dis,flag_set_0,flag_set_1,flag_set_2,vh,vl,flag_time=1;
uint count_f,dat_f;

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
	if(flag_dis==0)
	{
		buff_tube[0]=dig_code[time[2]/16];
		buff_tube[1]=dig_code[time[2]%16];
		buff_tube[2]=dig_code[10];
		buff_tube[3]=dig_code[time[1]/16];
		buff_tube[4]=dig_code[time[1]%16];
		buff_tube[5]=dig_code[10];
		buff_tube[6]=dig_code[time[0]/16];
		buff_tube[7]=dig_code[time[0]%16];
	}
	else if(flag_dis==1)
	{
		buff_tube[0]=dig_code[10];
		buff_tube[1]=dig_code[1];
		buff_tube[2]=dig_code[10];
		buff_tube[3]=dig_code[11];
		buff_tube[4]=dig_code[volta/100];
		buff_tube[5]=dig_code[volta%100/10];
		buff_tube[6]=dig_code[volta%10];
		buff_tube[7]=dig_code[0];
	}
	else if(flag_dis==2)
	{
		buff_tube[0]=dig_code[10];
		buff_tube[1]=dig_code[2];
		buff_tube[2]=dig_code[10];
		buff_tube[3]=dig_code[dat_f/10000];
		buff_tube[4]=dig_code[dat_f%10000/1000];
		buff_tube[5]=dig_code[dat_f%1000/100];
		buff_tube[6]=dig_code[dat_f%100/10];
		buff_tube[7]=dig_code[dat_f%10];
	}
	else if(flag_dis==3)
	{
		buff_tube[0]=dig_code[dat[0]/16];
		buff_tube[1]=dig_code[dat[0]%16];
		buff_tube[2]=dig_code[10];
		buff_tube[3]=dig_code[dat[1]/16];
		buff_tube[4]=dig_code[dat[1]%16];
		buff_tube[5]=dig_code[10];
		buff_tube[6]=dig_code[dat[2]/16];
		buff_tube[7]=dig_code[dat[2]%16];
	}
	else if(flag_dis==4)
	{
		
	}
	else if(flag_dis==5)
	{
		buff_tube[0]=dig_code[time[2]/10];
		buff_tube[1]=dig_code[time[2]%10];
		buff_tube[2]=dig_code[10];
		buff_tube[3]=dig_code[time[1]/10];
		buff_tube[4]=dig_code[time[1]%10];
		buff_tube[5]=dig_code[10];
		buff_tube[6]=dig_code[time[0]/10];
		buff_tube[7]=dig_code[time[0]%10];
	}
	else if(flag_dis==6)
	{
		buff_tube[0]=dig_code[vh/100];
		buff_tube[1]=dig_code[vh%100/10];
		buff_tube[2]=dig_code[vh%10];
		buff_tube[3]=dig_code[0];
		buff_tube[4]=dig_code[vl/100];
		buff_tube[5]=dig_code[vl%100/10];
		buff_tube[6]=dig_code[vl%10];
		buff_tube[7]=dig_code[0];
	}
	else if(flag_dis==7)
	{
		buff_tube[0]=dig_code[10];
		buff_tube[1]=dig_code[2];
		buff_tube[2]=dig_code[10];
		buff_tube[3]=dig_code[(1.0/dat_f*1000*1000)/10000];
		buff_tube[4]=dig_code[(1.0/dat_f*1000*1000)%10000/1000];
		buff_tube[5]=dig_code[(1.0/dat_f*1000*1000)%1000/100];
		buff_tube[6]=dig_code[(1.0/dat_f*1000*1000)%100/10];
		buff_tube[7]=dig_code[(1.0/dat_f*1000*1000)%10];
	}
}

uchar bcd_to_num(uchar bcd)
{
	uchar a,b;
	a=bcd/16;
	b=bcd%16;
	return (a*10+b);
}

uchar num_to_bcd(uchar num)
{
	uchar a,b;
	a=num/10;
	b=num%10;
	return (a*16+b);
}

void scan_key()
{
	static uchar key_buff[4][4]={{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff}};
	static uchar key_out=0;
	uchar i;
	key_buff[key_out][0]=(key_buff[key_out][0]<<1)|KEY_IN_0;
	key_buff[key_out][1]=(key_buff[key_out][1]<<1)|KEY_IN_1;
	key_buff[key_out][2]=(key_buff[key_out][2]<<1)|KEY_IN_2;
	key_buff[key_out][3]=(key_buff[key_out][3]<<1)|KEY_IN_3;
	for(i=0;i<4;i++)
	{
		if((key_buff[key_out][i]&0x0f)==0x0f)
		{
			key_stat[key_out][i]=1;
		}
		else if((key_buff[key_out][i]&0x0f)==0x00)
		{
			key_stat[key_out][i]=0;
		}
	}
	key_out++;
	key_out&=0x03;
	switch(key_out)
	{
		case 0: KEY_OUT_3=1;KEY_OUT_0=0;break;
		case 1: KEY_OUT_0=1;KEY_OUT_1=0;break;
		case 2: KEY_OUT_1=1;KEY_OUT_2=0;break;
		case 3: KEY_OUT_2=1;KEY_OUT_3=0;break;
		default: break;
	}
}

void key_fun(uchar i,uchar j)
{
	if(i==0&&j==0) //S7按键，显示时间
	{
		flag_dis=0;
		write_ds1302();
		flag_time=1;
	}
	else if(i==1&&j==0) //S6按键，显示电压阈值
	{
		flag_dis=1;
	}
	else if(i==2&&j==0) //S5按键，显示频率
	{
		flag_dis=2;
	}
	else if(i==2&&j==1) //S9按键，显示数据发生时间&类型
	{
		flag_dis=3;
	}
	else if(i==3&&j==0) //S4按键，切换设置/显示类型
	{
		if(flag_dis==0||flag_dis==5) //如果位于时间显示状态进入时间设置状态，位于时间设置界面则切换时间设置单元
		{
			flag_dis=5;
			flag_set_0++; //设置单元
			flag_time=0; //进入时间设置状态后关闭时间读取
			if(flag_set_0>2)
			{
				flag_set_0=0;
			}
		}
		else if(flag_dis==1||flag_dis==6) //如果位于电压显示状态进入阈值设置状态，位于阈值设置界面则切换阈值设置单元
		{
			flag_dis=6;
			flag_set_1=~flag_set_1;
		}
		else if(flag_dis==2||flag_dis==7) //如果处于频率显示，则切换为周期/频率显示
		{
			flag_dis=7;
			flag_set_2=~flag_set_2;
		}
	}
	else if(i==0&&j==1) //S11按键，加
	{
		if(flag_dis==5)
		{
			if(flag_set_0==0) //时间设置界面下小时单元的处理
			{
				time[2]++;
				if(time[2]>=24)
				{
					time[2]=0;
				}
			}
			else if(flag_set_0==1)
			{
				time[1]++;
				if(time[1]>=60)
				{
					time[1]=0;
				}
			}
			else if(flag_set_0==2)
			{
				time[0]++;
				if(time[0]>=60)
				{
					time[0]=0;
				}
			}
		}
		else if(flag_dis==6)
		{
			if(flag_set_1==0) //电压阈值设置状态下vh阈值的处理
			{
				vh=vh+500;
				if(vh>=5000)
				{
					vh=5000;
				}
			}
			else if(flag_set_1==1)
			{
				vl=vl+500;
				if(vl>=5000)
				{
					vl=5000;
				}
			}
		}
	}
	else if(i==1&&j==1) //S10按键，减，功能参考上文
	{
		if(flag_dis==5)
		{
			if(flag_set_0==0)
			{
				time[2]--;
				if(time[2]<=-1)
				{
					time[2]=23;
				}
			}
			else if(flag_set_0==1)
			{
				time[1]--;
				if(time[1]<=-1)
				{
					time[1]=59;
				}
			}
			else if(flag_set_0==2)
			{
				time[0]--;
				if(time[0]<=-1)
				{
					time[0]=59;
				}
			}
		}
		else if(flag_dis==6)
		{
			if(flag_set_1==0)
			{
				vh=vh-500;
				if(vh<=0)
				{
					vh=0;
				}
			}
			else if(flag_set_1==1)
			{
				vl=vl-500;
				if(vl<=0)
				{
					vl=0;
				}
			}
		}
	}
}

void key_press()
{
	uchar i,j;
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

void adc()
{
	IIC_Start();
	IIC_SendByte(0x90);
	IIC_WaitAck();
	IIC_SendByte(0x03);
	IIC_WaitAck();
	IIC_Stop();
	
	IIC_Start();
	IIC_SendByte(0x91);
	IIC_WaitAck();
	volta=(IIC_RecByte()/255.0*250)*2;
	IIC_Ack(0);
	IIC_Stop();
}

void write_time()
{
	uchar address=0x80;
	uchar i;
	Write_Ds1302(0x8e,0x00);
	for(i=0;i<3;i++)
	{
		Write_Ds1302(address,num_to_bcd(time[i]));
		address=address+2;
	}
	Write_Ds1302(0x8e,0x80);
}

void read_time()
{
	uchar address=0x81;
	uchar i;
	for(i=0;i<3;i++)
	{
		time[i]=bcd_to_num(Read_Ds1302(address));
		address=address+2;
	}
}

void write_data()
{
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(0x00);
	IIC_WaitAck();
	IIC_SendByte(time[2]);
	IIC_WaitAck();
	IIC_SendByte(time[1]);
	IIC_WaitAck();
	IIC_SendByte(time[0]);
	IIC_WaitAck();
	IIC_SendByte(type);
	IIC_WaitAck();
	IIC_Stop();
}
uchar read_data_byte(uchar address)
{
	uchar dat;
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(address);
	IIC_WaitAck();
	IIC_Stop();
	
	IIC_Start();
	IIC_SendByte(0xa1);
	IIC_WaitAck();
	dat=IIC_RecByte();
	IIC_Ack(0);
	IIC_Stop();
	return dat;
}

void read_data()
{
	dat[0]=read_data_byte(0x00);
	dat[1]=read_data_byte(0x01);
	dat[2]=read_data_byte(0x02);
	dat[3]=read_data_byte(0x03);
}

void init_sys()
{
	select(4);
	P0=0xff;
	select(5);
	P0=0x00;
	select(0);
	write_time();
}

void init_timer0()
{
	TMOD |= 0x04;
	TL0 = 0xff;
	TH0 = 0xff;
	TR0 = 1;
	EA = 1;
	ET0 = 1;
}

void init_timer1(void)		//1毫秒@12.000MHz
{
	AUXR &= 0xBF;		//定时器时钟12T模式
	TMOD &= 0x0F;		//设置定时器模式
	TL1 = 0x18;		//设置定时初值
	TH1 = 0xFC;		//设置定时初值
	TF1 = 0;		//清除TF1标志
	TR1 = 1;		//定时器1开始计时
	EA = 1;
	ET1 = 1;
}

void ser_timer0() interrupt 1
{
	count_f++;
}

void ser_timer1() interrupt 3
{
	static uint count_50ms,count_1s;
	count_50ms++;
	count_1s++;
	set_display();
	display();
	scan_key();
	key_press();
	if(count_50ms>=50)
	{
		count_50ms=0;
		if(flag_time)
		{
			read_time();
		}
		read_data();
		adc();
	}
	if(count_1s>=1000)
	{
		count_1s=0;
		dat_f=count_f;
		count_f=0;
	}
}

int main()
{
	init_sys();
	init_timer0();
	init_timer1();
	write_data();
	while(1)
	{
		
	}
}