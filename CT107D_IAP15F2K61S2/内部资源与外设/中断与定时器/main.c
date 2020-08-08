#include<reg52.h>
unsigned int count_t = 0; //定时器计数
unsigned int count_f = 0; //频率计数
unsigned int dat_f = 0; //频率数值储存

void Init_Sys() //初始化函数，涉及译码器，与非门与锁存器，可不参阅。
{
	P2 = (P2&0X1F)|0XA0; //仅改变最低三位的值，而不影响其它位电平。
	P0 = 0X00;
	P2 = (P2&0X1F)|0X80;
	P0 = 0XFF;
}

void Init_Timer()
{
	TMOD = 0x16; //定时器1，16位定时；定时器0，8位自动重装计数。
	TL0 = 0xff;
	TH0 = 0xff;
	TH1 = (65535-50000)/256;
  TL1 = (65535-50000)%256;
	
	ET0 = 1;
	ET1 = 1;
	EA = 1;
	TR0 = 1;
	TR1 = 1;
}

void Ser_Timer0() interrupt 1
{
	count_f++;
}

void Ser_Timer1() interrupt 3
{
	TH1 = (65535-50000)/256;
  TL1 = (65535-50000)%256;
	count_t++;
	if(count_t == 20) //一秒内count_f的计数
	{
		dat_f = count_f;
		count_f = 0;
		count_t = 0;
	}
}


/* 外部中断初始化与服务函数
void Init_INT0()
{
	EX0 = 1; //允许外部中断0，详见前文中断结构。
	IT0 = 1; //配置外部中断触发方式——下降沿
	EA = 1; //允许总中断
}

void Init_INT1()
{
	EX1 = 1;
	IT1 = 1;
	EA = 1;
}

void Ser_INT0() interrupt 0
{
	L1 = ~L1; //取反LED灯对应端口电平
}

void Ser_INT1() interrupt 2
{
	L2 = ~L2;
}
*/
int main()
{
	Init_Sys();
//	Init_INT0();
//	Init_INT1();
	Init_Timer();
	while(1)
	{
		if(dat_f>10)
		{
			P0 = 0XFE;
		}
		if(dat_f>100)
		{
			P0 = 0XFC;
		}
		if(dat_f>1000)
		{
			P0 = 0XF8;
		}
		if(dat_f>10000)
		{
			P0 = 0XF0;
		}
	}
}