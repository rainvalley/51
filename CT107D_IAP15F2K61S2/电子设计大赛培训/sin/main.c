/*
DAC:PCF8591 Over IIC
CT107D_IAP15F2K61S2
IRC Freq:12MHz
*/
#include<stc15f2k60s2.h>
#include"iic.h"
#include <intrins.h>
#define len 60
typedef unsigned int uint;
typedef unsigned char uchar;

sbit S4=P3^3;
sbit S5=P3^2;
uchar sin_tab[len]={
	128,141,154,167,180,191,203,213,223,231,238,244,249,253,255,255,255,253,249,245,238,231,223,213,203,192,180,167,154,141,128,114,101,88,76,64,53,42,33,24,17,11,6,2,0,0,0,2,6,10,16,24,32,41,52,63,75,87,100,114
};
uchar sin_status=1; //按键对应的波类型
float sin_range=1; //按键对应的波幅度
float freq; //波频率
float range; //波幅度
uchar flag=0;
	
void init_sys()
{
	P2=(P2&0x1F)|0xA0;	//选择到Y5端口
	P0=0x00;  //配置蜂鸣器
	P2=(P2&0x1F)|0x80;
	P0=0xff;	//配置LED灯熄灭
}

void Timer0Init()		//65毫秒@11.0592MHz
{
	float time=1000.0/freq*1000; //转换为定时累加单元的对应参数
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = ((65535-(int)time)/len)%256;		//设置定时初值
	TH0 = ((65535-(int)time)/len)/256;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
}

void Delay100us()		//@11.0592MHz
{
	unsigned char i, j;

	_nop_();
	_nop_();
	i = 2;
	j = 15;
	do
	{
		while (--j);
	} while (--i);
}

void DAC_Write(uchar DAT)
{
    IIC_Start();                  //I2C总线起始地址
    IIC_SendByte(0x90);				//发送PCF8591地址加读写方向位0（写）
    IIC_WaitAck();
	
    IIC_SendByte(0x40);     //发送控制字节DAC输出使能 
    IIC_WaitAck();              //检测是否发送成功(应答)
	
    IIC_SendByte(DAT);           //发送数字量交由PCF8591转为模拟量AOUT脚输出
    IIC_WaitAck();
	
    IIC_Stop();                   //I2C停止信号
}

void make_sin()	//正弦波参数配置，包括频率与幅度
{
	int i=0;
	for(i;i<len;i++)
	{
		sin_tab[i]=range*sin_tab[i];
		sin_tab[i]=(uchar)sin_tab[i];	//强制类型转换，共PCF8591读取
	}
	Timer0Init();
}

void ser_timer()interrupt 1 //中断服务函数内DAC输出正弦波
{
	if(flag==len+1)
	{
		flag=0;
	}
	DAC_Write(sin_tab[flag]);
	flag++;
}

void Scan_Keys()	//键盘扫描，循环配置波类型与幅度（S4修改波类型，S5修改幅度）
{
	if(S4==0)
	{
		Delay100us();
		if(S4==0)
		{
			if(sin_status==7)
			{
				sin_status=1;
			}
			sin_status++;
		}
	}	
	
	if(S5==0)
	{
		Delay100us();
		if(S5==0)
		{
			if(sin_range==1)
			{
				sin_range=0.1;
			}
			sin_range=sin_range+0.1;
		}
	}
}

void set_para()	//根据按键配置波参数
{
	Scan_Keys();
	switch(sin_status){
		case 1:
			freq=261.6;
			break;
		case 2:
			freq=293.6;
			break;
		case 3:
			freq=329.6;
			break;
		case 4:
			freq=349.2;
			break;
		case 5:
			freq=392;
			break;
		case 6:
			freq=440;
			break;
		case 7:
			freq=493.8;
			break;
	}
	range=sin_range;
}

int main()
{
	init_sys();
	while(1)
	{
		set_para();
		make_sin();
	}
}