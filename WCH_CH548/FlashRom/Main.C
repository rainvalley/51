#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
#include ".\Timer\Timer.H"
#include ".\PWM\PWM.H"
#define MINLEN 10
sbit LA=P2^0;
sbit LB=P2^1;
sbit RA=P2^2;
sbit RB=P2^3;
sbit Buzz=P1^7;
sbit RX=P1^4;
sbit TX=P1^5;

sbit AHEAD=P1^2;
sbit BACK=P1^3;

unsigned int time=0;
unsigned int dis=0;
unsigned char up=150;
unsigned char down=150;                                                     //PWM周期 FREQ_SYS/4/256

void left()
{	
	LA=0;
	LB=0;
	RA=1;
	RB=0;
}
void right()
{	
	LA=1;
	LB=0;
	RA=0;
	RB=0;
}
void ahead()
{
	LA=1;
	LB=0;
	RA=1;
	RB=0;
}
void back()
{
	LA=0;
	LB=1;
	RA=0;
	RB=1;
}

void speed_up()
{
	up++;
	mDelaymS(20);
	SetPWM3Dat(up);
	LB=0;
	SetPWM5Dat(up);
	LB=0;
	while(up==250)
	{
		ahead();
		up=150;
	}
}

void slow_down()
{
	down++;
	mDelaymS(20);
	LA=0;
	SetPWM4Dat(down);
	RA=0;
	SetPWM2Dat(down);
	while(down==250)
	{
		back();
		down=150;
	}
}
void measure()
{
	mTimer0Clk12DivFsys();
	TMOD=0X01;
	TL0=0X00;
	TH0=0X00;
	ET0 = 1;                                                                   //T0定时器中断开启
    EA = 1;

	TX=1;
	mDelayuS(15);
	TX=0;
	
	while(!RX);
	TR0=1;
	while(RX);
	TR0=0;
	time=(TH0*256+TL0)/2;
	dis=(time*1.78)/100;
}

void Wave()
{
	measure();
	if(dis<=MINLEN)
	{
		back();
		Buzz=1;
	}
	else if(dis>MINLEN)
	{
		ahead();
		Buzz=0;
	}
}


void control()
{
	if(AHEAD==1&&BACK==0)
	{
		ahead();
	}
	if(AHEAD==0&&BACK==1)
	{
		back();
	}
}


void main()
{
	P2_MOD_OC=0;//设置为推挽输出
	Buzz=0; 
	SetPWMClkDiv(4);                                                           //PWM时钟配置,FREQ_SYS/4
	SetPWMCycle256Clk(); 
	PWM_SEL_CHANNEL(CH3|CH5,Enable ); 
	/*while(1)
	{
		control();
		Wave();
	}*/
	while(1)
	{
		ahead();
	}
	
}



