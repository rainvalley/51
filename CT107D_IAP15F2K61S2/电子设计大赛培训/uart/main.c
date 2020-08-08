/*
2020-08-01 15：56
CT107D_IAP15F2K61S2
Baud：9600bps
*/
#include<stc15f2k60s2.h>
typedef unsigned char uchar;
typedef unsigned int uint;

void Select(uchar channel)
{
	switch(channel)
	{
		case 4:
			P2=(P2&0x1f)|0x80;
		break;
		case 5:
			P2=(P2&0x1f)|0xa0;
		break;
	}
}

void Init_Sys()
{
	Select(4);
	P0=0xff;
	Select(5);
	P0=0x00;
}

void Init_Uart(void)		//9600bps@11.0592MHz
{
	SCON = 0x50;		//8位数据,可变波特率
	AUXR &= 0xBF;		//定时器1时钟为Fosc/12,即12T
	AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0F;		//设定定时器1为16位自动重装方式
	TL1 = 0xE8;		//设定定时初值
	TH1 = 0xFF;		//设定定时初值
	ET1 = 0;		//禁止定时器1中断
	TR1 = 1;		//启动定时器1
	ES = 1;
	EA = 1;
}

void Ser_Uart() interrupt 4
{
	if(RI)
	{
		RI=0;
	}
}

void Send_String(uchar *str)
{
	while(*str!='\0')
	{
		SBUF=*str++;
		while(TI==0);
		TI=0;
	}
}

int main()
{
	Init_Sys();
	Init_Uart();
	Send_String("Last week I went to the theatre. I had a very good seat. The play was very interesting. I did not enjoy it. A young man and a young woman were sitting behind me. They were talking loudly. I got very angry. I could not hear the actors. I turned round. I looked at the man and the woman angrily. They did not pay any attention. In the end, I could not bear it. I turned round again. ‘I can't hear a word!’ I said angrily.‘It's none of your business, ’ the young man said rudely. ‘This is a private conversation!’");
	while(1);
}