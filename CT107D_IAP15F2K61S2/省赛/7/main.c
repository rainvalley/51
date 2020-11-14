#include<stc15f2k60s2.h>
#include"onewire.h"
typedef unsigned char uchar;
typedef unsigned int uint;
uchar code dig_code[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf,0xff,0xc6};
uchar buff_tube[]={0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
//三种模式下的倒计时间隔
uchar time[3];
//定义初始变量：长按、触发、工作模式、pwm生成标志位、温度显示标志位、按键标志位、1s标志位、pwm占空比、50ms标志位
uchar cont=0,trg=0,mode=1,flag_pwm=1,flag_temp=0,flag_key=0,flag_1s=0,pwm=1,flag_50ms=0;
uint temp=0;
sbit SIG=P3^4;
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
	select(4);
	P0=0xff;
	select(5);
	P0=0x00;
	select(0);
}


void display(void)
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

void key_fun()
{
	if(trg==0x01)
	{
		mode++;
		if(mode>=4)
		{
			mode=1;
		}
	}
	if(trg==0x02)
	{
		if(flag_key==0)
		{
			time[mode-1]=0;
			flag_key=1;
		}
		else if(flag_key==1)
		{
			time[mode-1]=60;
			flag_key=2;
		}
		else if(flag_key==2)
		{
			time[mode-1]=120;
			flag_key=0;
		}
		flag_pwm=1;
	}
	if(trg==0x04)
	{
		time[mode-1]=0;
		flag_pwm=0;
	}
	if(trg==0x08)
	{
		flag_temp=~flag_temp;
	}
}

void set_tube()
{
	if(flag_temp)
	{
		buff_tube[0]=dig_code[10];
		buff_tube[1]=dig_code[4];
		buff_tube[2]=dig_code[10];
		buff_tube[3]=dig_code[11];
		buff_tube[4]=dig_code[11];
		buff_tube[5]=dig_code[temp/10];
		buff_tube[6]=dig_code[temp%10];
		buff_tube[7]=dig_code[12];
	}
	else
	{
		buff_tube[0]=dig_code[10];
		buff_tube[1]=dig_code[mode];
		buff_tube[2]=dig_code[10];
		buff_tube[3]=dig_code[11];
		buff_tube[4]=dig_code[0];
		buff_tube[5]=dig_code[time[mode-1]/100];
		buff_tube[6]=dig_code[time[mode-1]%100/10];
		buff_tube[7]=dig_code[time[mode-1]%10];
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
	temp=(MSB<<8)|LSB;
	temp=temp>>4;
}

void set_pwm()
{
	if(mode==1)
	{
		pwm=2; //睡眠风
	}
	else if(mode==2)
	{
		pwm=3; //自然风
	}
	else if(mode==3)
	{
		pwm=7; //常风
	}
}

void set_led()
{
	if(mode==1&&flag_pwm)
	{
		select(4);
		P0=0xfe;
		select(0);
	}
	else if(mode==2&&flag_pwm)
	{
		select(4);
		P0=0xfd;
		select(0);
	}
	else if(mode==3&&flag_pwm)
	{
		select(4);
		P0=0xfb;
		select(0);
	}
	else if(!flag_pwm) //如果停止工作
	{
		select(4);
		P0=0xff;
		select(0);
	}
}

void init_timer0(void)		//100微秒@11.0592MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0xA4;		//设置定时初值
	TH0 = 0xFF;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;
	EA = 1;
}

void ser_timer0(void) interrupt 1
{
	static uint count_50ms=0,count_1s=0;
	static uchar count=0,count_2ms=0;
	count++;
	count_50ms++;
	count_1s++;
	count_2ms++;
	if(count_2ms==20)
	{
		display();
		count_2ms=0;
	}
	if(flag_pwm)
	{
		if(count<=pwm)
		{
			SIG=1;
		}
		else if(count<10)
		{
			SIG=0;
		}
		else if(count==10)
		{
			count=0;
		}
	}
	if(count_50ms==500) //50ms时扫描按键，执行按键功能，设置占空比
	{
		count_50ms=0;
		flag_50ms=1;
	}
	if(count_1s==10000) //1s时读取温度，递减当前定时器
	{
		count_1s=0;
		flag_1s=1;
	}
}

void set_led_off()
{
	if(time[mode-1]<=0) //如果时间减少到0s则停止pwm波的生成
	{
		flag_pwm=0;
	}
	else if(time[mode-1]>120) //如果时间大于120s则将数据非法归零
	{
		time[mode-1]=0;
		flag_pwm=0;
	}
}

void main()
{
	init_sys();
	init_timer0();
	while(1)
	{
		set_tube();
		set_led();
		set_led_off(); 
		if(flag_1s)
		{
			get_temp();
			if(flag_pwm) //如果正在运行
			{
				time[mode-1]=time[mode-1]-1;
			}
			flag_1s=0;
		}
		if(flag_50ms)
		{
			scan_key();
			key_fun();
			set_pwm();
			flag_50ms=0;
		}
	}
}