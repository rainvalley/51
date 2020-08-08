#include<stc15f2k60s2.h>
typedef unsigned int uint;
typedef unsigned char uchar;
uchar code dig_code[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0x88,0x80,0xc6,0xc0,0x86,0x8e,0xbf,0x7f,0xff};
sbit KEY_IN_1 = P4^4;
sbit KEY_IN_2 = P4^2;
sbit KEY_IN_3 = P3^5;
sbit KEY_IN_4 = P3^4;
sbit KEY_OUT_1 = P3^0;
sbit KEY_OUT_2 = P3^1;
sbit KEY_OUT_3 = P3^2;
sbit KEY_OUT_4 = P3^3;
uchar KeySta[4][4]={{1,1,1,1},{1,1,1,1},{1,1,1,1},{1,1,1,1}};
void delay(uint t)
{
	while(t--);
}

void select(uchar channel)
{
	switch(channel)
	{
		case 4: 
			P2=(P2&0X1f)|0X80;//不改变P2其他IO口电平的情况下改变138译码器输入
			break;
		case 5: 
			P2=(P2&0X1f)|0Xa0;
			break;
		case 6: 
			P2=(P2&0X1f)|0Xc0;
			break;
		case 7: 
			P2=(P2&0X1f)|0Xe0;
			break;
	}
		
}

void display(uchar pos,uchar dig)
{
	delay(2000);
	P0=0XFF;
	select(6);
	P0=0X01<<pos;
	select(7);
	P0=dig_code[dig];
}

void InitSys()
{
	select(5);
	P0=0X00;
	select(4);
	P0=0XFF;
}

void Timer0Init(void)		//1毫秒@11.0592MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x66;		//设置定时初值
	TH0 = 0xFC;		//设置定时初值设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;
	EA = 1;
}


void Scan_Key_16(void)
{
	uchar i;
	static uchar keyout = 0;		//矩阵按键扫描输出行索引
	static uchar keybuff[4][4] = {{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff}};	//矩阵按键扫描缓存区
	
	keybuff[keyout][0] = (keybuff[keyout][0] << 1) | KEY_IN_1;		//将每一行的4个按键值移入缓存区
	keybuff[keyout][1] = (keybuff[keyout][1] << 1) | KEY_IN_2;
	keybuff[keyout][2] = (keybuff[keyout][2] << 1) | KEY_IN_3;
	keybuff[keyout][3] = (keybuff[keyout][3] << 1) | KEY_IN_4;
		
	//消抖后更新按键状态
	for(i = 0;i < 4;i ++)
	{
		if((keybuff[keyout][i] & 0x0f) == 0x00)
			KeySta[keyout][i] = 0;		//连续4次扫描值都是0，即4×4ms内都是按下状态，认为按键已平稳按下
		else if((keybuff[keyout][i] & 0x0f) == 0x0f)
			KeySta[keyout][i] = 1;		//连续4次扫描值都是1，即4×4ms内都是松开状态，认为按键已稳定弹起
	}
	
	//执行下一次的扫描输出
	keyout ++;
	keyout = keyout & 0x03;		//索引加到4就归零
	switch(keyout)						//根据索引，释放当前输出引脚，拉低下次的输出引脚
	{
		case 0:KEY_OUT_4 = 1;KEY_OUT_1 = 0;break;
		case 1:KEY_OUT_1 = 1;KEY_OUT_2 = 0;break;
		case 2:KEY_OUT_2 = 1;KEY_OUT_3 = 0;break;
		case 3:KEY_OUT_3 = 1;KEY_OUT_4 = 0;break;
		default:break;
	}

}

void Ser_Timer0() interrupt 1
{
	Scan_Key_16();
}

int main()
{
	InitSys();
	P3 = 0XFF;
	Timer0Init();
	while(1)
	{
		display(0,KeySta[0][0]);
		display(1,KeySta[1][1]);
		display(2,KeySta[2][2]);
		display(3,KeySta[3][3]);
	}
}