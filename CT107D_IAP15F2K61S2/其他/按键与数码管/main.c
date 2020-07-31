#include<stc15f2k60s2.h>
typedef unsigned int uint;
typedef unsigned char uchar;
sbit S4=P3^3;
sbit S5=P3^2;
sbit S6=P3^1;
sbit S7=P3^0;
uchar code Dig_Code[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xBF,0XFF};
uchar mode=1;
uint gap=400;
uchar start=1;
uchar selected=0;
uchar count_t=0;
uchar stat=0;
void Display_Info();
void Display_Blink(uchar stat);

void Delay(uint t)
{
	while(t--);
}

void Select(uchar channel)
{
	switch(channel)
	{
		case 4:
			P2=(P2&0X1F)|0X80;
		break;
		case 5:
			P2=(P2&0X1F)|0XA0;
		break;
		case 6:
			P2=(P2&0X1F)|0XC0;
		break;
		case 7:
			P2=(P2&0X1F)|0XE0;
		break;
		case 0:
			P2=(P2&0X1F)|0X00;
		break;
	}
}

void Init_Sys()
{
	Select(5);
	P0=0X00;
	Select(4);
	P0=0XFF;
}

void Init_Timer0()
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

void Display(uchar pos,uchar num)
{
	Delay(1500);
	P0=0XFF;
	Select(6);
	P0=0X01<<pos;
	Select(7);
	P0=Dig_Code[num];
}

void Scan_Key()
{
	if(S7==0)
	{
		Delay(200);
		if(S7==0)
		{
			while(S7==0)
			{
				Display_Info();
				Display_Blink(stat);
			}
			start=~start;
		}
	}
	
	if(S6==0)
	{
		Delay(200);
		if(S6==0)
		{
			while(S6==0)
			{
				Display_Info();
				Display_Blink(stat);
			}
			selected=selected+1;
			if(selected==3)
			{
				selected=0;
			}
		}
	}
	
	if(S5==0)
	{
		Delay(200);
		if(S5==0)
		{
			while(S5==0)
			{
				Display_Info();
				Display_Blink(stat);
			}
			if(selected==1&&mode<=3)
			{
				mode=mode+1;
			}
			if(selected==2&&gap<=1100)
			{
				gap=gap+100;
			}
		}
	}
	
	if(S4==0)
	{
		Delay(200);
		if(S4==0)
		{
			while(S4==0)
			{
				Display_Info();
				Display_Blink(stat);
			}
			if(selected==1&&mode>=2)
			{
				mode=mode-1;
			}
			if(selected==2&&gap>=500)
			{
				gap=gap-100;
			}
		}
	}
}

void Display_Off()
{
	Select(6);
	P0=0XFF;
	Select(7);
	P0=0XFF;
}

void Display_Info()
{
	if(selected==1)
	{
		Display(4,gap/1000);
		Display(5,(gap%1000)/100);
		Display(6,0);
		Display(7,0);
		Display(7,11);
	}
	else if(selected==2)
	{
		Display(0,10);
		Display(1,mode);
		Display(2,10);
		Display(2,11);//避免刷新时该位闪烁
	}
	else if(selected==0)
	{
		Display_Off();
	}
}

void Display_Blink(uchar stat)
{
	if(stat)
	{
		if(selected==1)
		{
			Display(0,10);
			Display(1,mode);
			Display(2,10);
		}
		else if(selected==2)
		{
			Display(4,gap/1000);
			Display(5,(gap%1000)/100);
			Display(6,0);
			Display(7,0);
		}
	}
	else
	{
		if(selected==1)
		{
			Select(6);
			P0=0X0F;
			Select(7);
			P0=0XFF;
		}
		else if(selected==2)
		{
			Select(6);
			P0=0XF0;
			Select(7);
			P0=0XFF;
		}
	}
}

void Ser_Timer0() interrupt 1
{
	count_t++;
	if(count_t==16)
	{
		stat=~stat;
		count_t=0;
	}
}

int main()
{
	Init_Sys();
	Init_Timer0();
	while(1)
	{
		Scan_Key();
		Display_Info();
		Display_Blink(stat);
	}
}	