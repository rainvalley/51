#include<stc15f2k60s2.h>
#include<intrins.h>
typedef unsigned char uchar;
typedef unsigned int uint;
uchar code dig_code[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x86};
sbit TX=P1^0; //发送引脚
sbit RX=P1^1; //接收引脚
uint distance=0;
void delay(uint t)
{
	while(t--);
}

void delay12us()
{
	unsigned char i;

	_nop_();
	_nop_();
	_nop_();
	i = 33;
	while (--i);
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

void display(uchar pos,uchar value)
{
	delay(1000);
	P0=0xff;
	select(6);
	P0=0x01<<pos;
	select(7);
	P0=dig_code[value];
}

void send_wave()
{
	uchar i;
	for(i=0;i<8;i++) //发送8个40KHz的超声波
	{
		TX=1;
		delay12us();
		TX=0;
		delay12us();
	}
}

void measure_distance()
{
	uint time=0;
	TMOD|=0x10;
	TL1=0;
	TH1=0;
	TF1=0;
	ET1=0;
	
	send_wave(); //开始发送并启动定时器统计接收的时间
	TR1=1;
	while((RX==1)&&(TF1==0)); //当未接收到反射且定时器未溢出时，死循环
	TR1=0;
	
	if(TF1==0) //定时器未溢出，根据反射时间计算举例
	{
		time=(TH1<<8)|TL1;
		distance=((time/10)*17)/100+3;
	}
	else //定时器溢出，超过最大量程
	{
		TF1=0;
		distance=999;
	}
}

void display_distance()
{
	if(distance==999)
	{
		display(0,10);
	}
	else
	{
		display(0,distance/100);
		display(1,distance%100/10);
		display(2,distance%10);
	}
}

void init_timer0(void)		//50毫秒@11.0592MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x00;		//设置定时初值
	TH0 = 0x4C;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;
	EA = 1;
}

void ser_timer0() interrupt 1
{
	static uchar count=0;
	if(count==4)
	{
		count=0;
		measure_distance();
	}
	count++;
}	


int main()
{
	init_sys();
	init_timer0();
	while(1)
	{
		display_distance();
	}
}