#include<stc15f2k60s2.h>
#include<iic.h>
typedef unsigned int uint;
typedef unsigned char uchar;
sbit S7=P3^0;
sbit S6=P3^1;
sbit S5=P3^2;
sbit S4=P3^3;
uchar code SMG[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xc1,0x8e};
uchar code SMG_Dot[]={0x40,0x79,0x24,0x30,0x19,0x12,0x02,0x78,0x00,0x10};//有点的段码等于原短码最高位变为0，减去8。

uint count_f=0;
uchar count_t=0;
uint dat_f=0;
uint dat_v=0;
uint dat_rb2=0;

bit F_DIS=0;
bit F_VOL=0;
bit F_SMG=0;
bit F_LED=0;
bit F_DAC=0;

void delay(uint t)
{
	while(t--);
}

void select(uchar channel)
{
	switch(channel)
	{
		case 4: P2=(P2&0X1F)|0X80;break;
		case 5: P2=(P2&0X1F)|0XA0;break;
		case 6: P2=(P2&0X1F)|0XC0;break;
		case 7: P2=(P2&0X1F)|0XE0;break;
		case 0: P2=P2&0X1F;break;
	}
}

void display_bit(uchar pos,uchar num)
{
	P0=0XFF;
	select(6);
	P0=0X01<<pos;
	select(7);
	P0=num;
}

void ADC()
{
	IIC_Start();
	IIC_SendByte(0X90);
	IIC_WaitAck();
	IIC_SendByte(0X43);
	IIC_WaitAck();
	IIC_Stop();
	
	IIC_Start();
	IIC_SendByte(0X91);
	IIC_WaitAck();
	dat_rb2=IIC_RecByte();
	IIC_SendAck(1);
	IIC_Stop();
	
	dat_v=dat_rb2*2;
}

void DAC(uchar dat)
{
	IIC_Start();
	IIC_SendByte(0x90);
	IIC_WaitAck();
	IIC_SendByte(0x40);
	IIC_WaitAck();
	IIC_SendByte(dat);
	IIC_SendAck(1);
	IIC_Stop();
}

void Init_Timer()
{
	TL0=0XFF;
	TH0=0XFF;
	TL1=(65535-50000)/256;
	TH1=(65535-50000)%256;
	
	TMOD=0X16;
	ET0=1;
	ET1=1;
	EA=1;
	TR0=1;
	TR1=1;
}

void ser_t0() interrupt 1
{
	count_f++;
}

void ser_t1() interrupt 3
{
	count_t++;
	if(count_t%10==0)
	{
		F_DAC=~F_DAC;
	}
	if(count_t==20)
	{
		dat_f=count_f;
		count_f=0;
		count_t=0;
	}
}

void display_u()
{
	display_bit(0,SMG[10]);
	delay(500);
	display_bit(5,SMG_Dot[(dat_v/100)%10]);
	delay(500);
	display_bit(6,SMG[(dat_v/10)%10]);
	delay(500);
	display_bit(7,SMG[(dat_v)%10]);
}

void display_f()
{
	display_bit(0,SMG[11]);
	delay(500);
	display_bit(3,SMG[dat_f/10000]);
	delay(500);
	display_bit(4,SMG[(dat_f/1000)%10]);
	delay(500);
	display_bit(5,SMG[(dat_f/100)%10]);
	delay(500);
	display_bit(6,SMG[(dat_f/10)%10]);
	delay(500);
	display_bit(7,SMG[dat_f%10]);
}

void led_u()
{
	select(0);
	if(dat_v<150)
		P0|=0X04;
	else if(dat_v<250)
		P0&=~0X04;
	else if(dat_v<350)
		P0|=0X04;
	else 
		P0&=~0X04;
	select(4);
}

void led_f()
{
	select(0);
	if(dat_f<1000)
		P0|=0X08;
	else if(dat_f<5000)
		P0&=~0X08;
	else if(dat_f<10000)
		P0|=0X08;
	else
		P0&=~0X08;
	select(4);
}

void led()
{
	select(0);
	if(F_LED==0)
	{
		if(F_DIS==0)
		{
			P0&=0XFE;
			led_u();
		}
		else
		{
			P0&=0XFD;
			led_f();
		}
		
		if(F_VOL==0)
			P0|=0X10;
		else
			P0&=~0X10;
	}
	else
		P0=0XFF;
	select(4);
}

void display()
{
	if(F_SMG==0)
	{
		if(F_DIS==0)
			display_u();
		else
			display_f();
	}
	else
	{
		select(6);
		P0=0XFF;
		select(7);
		P0=0XFF;
	}
	led();
	select(0);
}

void scan()
{
	if(S4==0)
	{
		delay(100);
		if(S4==0)
		{
			F_DIS=~F_DIS;
			while(S4==0)
				display();
		}
	}
	if(S5==0)
	{
		delay(100);
		if(S5==0)
		{
			F_VOL=~F_VOL;
			while(S5==0)
				display();
		}
	}
	if(S6==0)
	{
		delay(100);
		if(S6==0)
		{
			F_LED=~F_LED;
			while(S6==0)
				display();
		}
	}
	if(S7==0)
	{
		delay(100);
		if(S7==0)
		{
			F_SMG=~F_SMG;
			while(S7==0)
				display();
		}
	}
}

void initsys()
{
	select(5);
	P0=0X00;
	select(4);
	P0=0XFF;
	select(0);
	Init_Timer();
}



int main()
{
	initsys();
	while(1)
	{
		scan();
		if(F_DAC==0)
		{
			ADC();
		}
		else
		{
			if(F_VOL==0)
			{
				DAC(102);
			}
			else
			{
				DAC(dat_rb2);
			}
		}
		display();
	}
}