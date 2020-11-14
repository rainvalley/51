#include<stc15f2k60s2.h>
#include"onewire.h"
typedef unsigned char uchar;
typedef unsigned int uint;
//数码管段码与按键分布
uchar code dig_code[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf,0xff,0xc6};
uchar code key_map[4][4]={{0,1,2,19},{3,4,5,18},{6,7,8,17},{9,10,11,16}}; 
//矩阵按键状态与备份
uchar key_stat[4][4]={{1,1,1,1},{1,1,1,1},{1,1,1,1},{1,1,1,1}};
uchar key_back[4][4]={{1,1,1,1},{1,1,1,1},{1,1,1,1},{1,1,1,1}};
//数码管与按键缓冲区
uchar buff_tube[8]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
uchar buff[4]={11,11,11,11};
//逐行扫描
sbit KEY_IN_0=P4^4;
sbit KEY_IN_1=P4^2;
sbit KEY_IN_2=P3^5;
sbit KEY_IN_3=P3^4;
sbit KEY_OUT_0=P3^0;
sbit KEY_OUT_1=P3^1;
sbit KEY_OUT_2=P3^2;
sbit KEY_OUT_3=P3^3;
sbit L1=P0^0;
sbit L2=P0^1;
sbit relay=P0^4;
//变量：最低温度、最高温度、当前工作模式、当前的温度模式
uchar tmin=20,tmax=30,mode=0,mode_temp=0;
//一堆标志变量
uchar flag_1ms,flag_200ms,flag_400ms,flag_800ms,flag_error,flag_1s;
uint temp=0;

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
		case 0:
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
	static uchar key_buff[4][4]={{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff}};
	static uchar key_out=0; //行索引
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
		default:break;
	}
}

void key_fun(uchar value)
{
	static uchar pos=0; //按键位置
	if(value>=0&&value<=9) //如果按下0~9并且处于设置模式则对buff进行修改
	{
		if(mode==1)
		{
			buff[pos]=value;
			pos++;
			if(pos>=4)
			{
				pos=4;
			}
		}
	}
	if(value==10) //如果按下设置按键则根据情况改变工作模式
	{
		if(mode==0)
		{
			mode=1;
		}
		else if(mode==1)
		{
			if(tmin=buff[2]*10+buff[3]>=buff[0]*10+buff[1]) //参数设置非法
			{
				flag_error=1;
				mode=1;
			}
			else
			{
				mode=0;
				tmax=buff[0]*10+buff[1];
				tmin=buff[2]*10+buff[3];
				buff[0]=buff[1]=buff[2]=buff[3]=11;
				pos=0;
				flag_error=0;
			}
		}
	}
	if(value==11&&mode==1) //如果在设置模式下按下清除
	{
		pos=0;
		buff[0]=buff[1]=buff[2]=buff[3]=11;
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
					key_fun(key_map[i][j]);
				}
				key_back[i][j]=key_stat[i][j];
			}
		}
	}
}

void set_display()
{
	if(mode==0)
	{
		buff_tube[0]=dig_code[10];
		buff_tube[1]=dig_code[mode_temp];
		buff_tube[2]=dig_code[10];
		buff_tube[3]=dig_code[11];
		buff_tube[4]=dig_code[11];
		buff_tube[5]=dig_code[11];
		buff_tube[6]=dig_code[temp/10];
		buff_tube[7]=dig_code[temp%10];
	}
	else
	{
		buff_tube[0]=dig_code[10];
		buff_tube[1]=dig_code[buff[0]];
		buff_tube[2]=dig_code[buff[1]];
		buff_tube[3]=dig_code[11];
		buff_tube[4]=dig_code[11];
		buff_tube[5]=dig_code[10];
		buff_tube[6]=dig_code[buff[2]];
		buff_tube[7]=dig_code[buff[3]];
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

void ser_timer0()interrupt 1
{
	static uint count_2ms,count_200ms,count_400ms,count_800ms,count_1s;
	count_2ms++;
	count_200ms++;
	count_400ms++;
	count_800ms++;
	count_1s++;
	flag_1ms=1;
	if(count_2ms>=2)
	{
		count_2ms=0;
		set_display();
		display();
	}
	if(count_200ms>=200)
	{
		count_200ms=0;
		flag_200ms=~flag_200ms;
	}
	if(count_400ms>=400)
	{
		count_400ms=0;
		flag_400ms=1;
	}
	if(count_800ms>=800)
	{
		count_800ms=0;
		flag_800ms=~flag_800ms;
	}
	if(count_1s>=1000)
	{
		count_1s=0;
		flag_1s=1;
	}
}

void led_on(uchar dat)
{
	P0=0xff;
	select(4);
	P0=dat;
	select(0);
}

void led_fun()
{
	if(flag_error)
	{
		P0=0xff;
		select(4);
		L2=0;
		select(0);
	}
	else if(flag_error==0)
	{
		P0=0xff;
		select(4);
		L2=1;
		select(0);
	}
	
	if(temp<tmin)
	{
		mode_temp=0;
		P0=0x00;
		select(5);
		relay=0;
		select(0);
		if(flag_800ms)
		{
			led_on(0xfe);
		}
		else
		{
			led_on(0xff);
		}
	}
	else if(temp<tmax)
	{
		mode_temp=1;
		P0=0x00;
		select(5);
		relay=0;
		select(0);
		if(flag_400ms)
		{
			led_on(0xfe);
		}
		else
		{
			led_on(0xff);
		}
	}
	else
	{
		mode_temp=2;
		P0=0x00;
		select(5);
		relay=1;
		select(0);
		if(flag_200ms)
		{
			led_on(0xfe);
		}
		else
		{
			led_on(0xff);
		}
	}
}

void get_temp()
{
	uchar LSB,MSB;
	EA=0;
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0x44);
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0xbe);
	LSB=Read_DS18B20();
	MSB=Read_DS18B20();
	init_ds18b20();
	temp=(MSB<<8)|LSB;
	temp=temp>>4;
	EA=1;
}

void main()
{
	init_sys();
	init_timer0();
	while(1)
	{
		if(flag_1ms)
		{
			flag_1ms=0;
			led_fun();
			scan_key();
			key_press();
		}
		if(flag_1s)
		{
			flag_1s=0;
			get_temp();
		}
	}
}