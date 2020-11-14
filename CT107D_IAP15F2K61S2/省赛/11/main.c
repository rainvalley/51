#include <stc15f2k60s2.h>
#include "iic.h"
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
uchar dig_code[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xc1,0x8c,0xc8,0xff};
uchar buff_tube[]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
uchar key_stat[4][4]={{1,1,1,1},{1,1,1,1},{1,1,1,1},{1,1,1,1}};
uchar key_back[4][4]={{1,1,1,1},{1,1,1,1},{1,1,1,1},{1,1,1,1}};
uchar flag_40ms,flag_mode,flag_L2,flag_error,count_error,flag_5s,flag_5s_start;
uint volta,count,vp,v_back;
void read_data();

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
	read_data();
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
	if(flag_mode==0)
	{
		buff_tube[0]=dig_code[10];
		buff_tube[1]=dig_code[13];
		buff_tube[2]=dig_code[13];
		buff_tube[3]=dig_code[13];
		buff_tube[4]=dig_code[13];
		buff_tube[5]=dig_code[volta/100]+0x80;
		buff_tube[6]=dig_code[volta%100/10];
		buff_tube[7]=dig_code[volta%10];
	}
	else if(flag_mode==1)
	{
		buff_tube[0]=dig_code[11];
		buff_tube[1]=dig_code[13];
		buff_tube[2]=dig_code[13];
		buff_tube[3]=dig_code[13];
		buff_tube[4]=dig_code[13];
		buff_tube[5]=dig_code[vp/100]+0x80;
		buff_tube[6]=dig_code[vp%100/10];
		buff_tube[7]=dig_code[vp%10];
	}
	else if(flag_mode==2)
	{
		buff_tube[0]=dig_code[12];
		buff_tube[1]=dig_code[13];
		buff_tube[2]=dig_code[13];
		buff_tube[3]=dig_code[13];
		buff_tube[4]=dig_code[13];
		buff_tube[5]=dig_code[13];
		buff_tube[6]=dig_code[count/10];
		buff_tube[7]=dig_code[count%10];
	}
}

void save_data()
{
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(0x00);
	IIC_WaitAck();
	IIC_SendByte(vp/10);
	IIC_WaitAck();
	IIC_Stop();
}

void read_data()
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
	vp=IIC_RecByte()*10;
	IIC_Ack(0);
	IIC_Stop();
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
		case 0:KEY_OUT_3=1;KEY_OUT_0=0;break;
		case 1:KEY_OUT_0=1;KEY_OUT_1=0;break;
		case 2:KEY_OUT_1=1;KEY_OUT_2=0;break;
		case 3:KEY_OUT_2=1;KEY_OUT_3=0;break;
		default:break;
	}
}

void key_fun(uchar i,uchar j)
{
	if(i==3&&j==2)
	{
		flag_mode++;
		flag_error=0;
		if(flag_mode>=3)
		{
			flag_mode=0;
		}
		if(flag_mode==2)
		{
			save_data();
		}
	}
	else if(i==2&&j==2)
	{
		if(flag_mode==2)
		{
			count=0;
			flag_error=0;
		}
	}
	else if(i==3&&j==3)
	{
		if(flag_mode==1)
		{
			flag_error=0;
			if(vp>=500)
			{
				vp=0;
			}
			else
			{
				vp=vp+50;
			}
		}
	}
	else if(i==2&&j==3)
	{
		if(flag_mode==1)
		{
			flag_error=0;
			if(vp<=0)
			{
				vp=500;
			}
			else
			{
				vp=vp-50;
			}
		}
	}
	else
	{
		count_error++;
		if(count_error>=3)
		{
			flag_error=1;
			count_error=0;
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

void check_volta()
{
	uchar flag_up,flag_down;
	if(volta>vp)
	{
		flag_5s_start=0;
		flag_5s=0;
		flag_up=1;
		flag_down=0;
	}
	if(volta<vp)
	{
		flag_5s_start=1;
		flag_down=1;
	}
	if((flag_up==1)&&(flag_down==1))
	{
		count++;
		flag_up=0;
		flag_down=0;
	}
}

void set_flag()
{
	if(count/2==0)
	{
		flag_L2=0;
	}
	else
	{
		flag_L2=1;
	}
}

void set_led()
{
	P0=0xff;
	select(4);
	if(flag_5s)
	{
		P0&=0xfe;
	}
	else
	{
		P0|=~0xfe;
	}
	if(flag_L2)
	{
		P0&=0xfd;
	}
	else
	{
		P0|=~0xfd;
	}
	
	if(flag_error)
	{
		P0&=0xfb;
	}
	else
	{
		P0|=~0xfb;
	}
	select(0);
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

void init_timer0()		//1毫秒@12.000MHz
{
	AUXR |= 0x80;		//定时器时钟1T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x20;		//设置定时初值
	TH0 = 0xD1;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;
	EA = 1;
}

void ser_timer0() interrupt 1
{
	static uchar count_40ms,count_2ms;
	static uint count_5s;
	count_2ms++;
	count_40ms++;
	scan_key();
	key_press();
	if(flag_5s_start)
	{
		count_5s++;
		if(count_5s>=5000)
		{
			count_5s=0;
			flag_5s=1;
		}
	}
	if(count_40ms>=40)
	{
		count_40ms=0;
		flag_40ms=~flag_40ms;
	}
	if(count_2ms>=2)
	{
		count_2ms=0;
		set_display();
		display();
		set_flag();
		set_led();
	}
}

int main()
{
	init_sys();
	init_timer0();
	while(1)
	{
		if(flag_40ms)
		{
			adc();
			check_volta();
		}
	}
}