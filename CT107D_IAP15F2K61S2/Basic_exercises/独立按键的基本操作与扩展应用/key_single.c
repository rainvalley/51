#include<reg51.h>
typedef unsigned char uchar;
typedef unsigned int uint;
uchar stat_k=0;
sbit S7 = P3^0;
sbit S6 = P3^1;
sbit S5 = P3^2;
sbit S4 = P3^3;
sbit L1 = P0^0;
sbit L2 = P0^1;
sbit L3 = P0^2;
sbit L4 = P0^3;
sbit L5 = P0^4;
sbit L6 = P0^5;

void Delay(uint t)
{
	while(t--);
}


void SelectHC573(uchar channel)
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
	}
}

void Judge_key_singal()
{
	if(S7==0)
	{
		Delay(100);
		if(S7==0)
		{
			if(stat_k==0)
			{
				L1=0;
				stat_k=1;
			}
			else if(stat_k==1)
			{
				L1=1;
				stat_k=0;
			}
			while(S7==0);//若未再次按下则一直执行死循环，避免重复判断。
		}
	}	
	if(S6==0)
	{
		Delay(100);
		if(S6==0)
		{
			if(stat_k==0)
			{
				L2=0;
				stat_k=2;
			}
			else if(stat_k==2)
			{
				L2=1;
				stat_k=0;
			}
			while(S6==0);
		}
	}	
	if(S5==0)
	{
		Delay(100);
		if(S5==0)
		{
			if(stat_k==1)
			{
				L3=0;
				while(S5==0);
				L3=1;
			}
			else if(stat_k==2)
			{
				L5=0;
				while(S5==0);
				L5=1;
			}
		}
	}
	if(S4==0)
	{
		Delay(100);
		if(S4==0)
		{
			if(stat_k==1)
			{
				L4=0;
				while(S4==0);
				L4=1;
			}
			else if(stat_k==2)
			{
				L6=0;
				while(S4==0);
				L6=1;
			}
		}
	}		
}

int main()
{
	SelectHC573(5);
	P0=0x00;
	SelectHC573(4);
	P0=0xff;
	while(1)
	{
		  Judge_key_singal();
	}
}