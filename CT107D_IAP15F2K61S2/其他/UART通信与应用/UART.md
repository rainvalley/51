# 协议划分
设备之间的通信大体上分为串行与并行
* 串行：数据位按照**顺序**发送或接收，常见有UART，IIC，SPI，OneWire等，占主流。
* 并行：数据位**同时**发送或接收，常见于CPU周围芯片的连接，速度快但对设计要求高。

按照传输模式划分：单工，半双工，全双工
* 单工：仅能发送或者接收。
* 半双工：能发送也能接收，但一次通信中只能发送或者接收。
* 全双工：发送数据的同时也能够接收数据，二者同步进行。

按照时钟信号划分：同步，异步
* 同步：通信双方使用同一个时钟源，以数据块为单位发送
* 异步：每个设备都拥有自己的时钟源，在通信过程中需要保证双方的波特率同步，以数据帧为单位发送

# UART
UART是典型的异步全双工串行通信，仅使用两根数据线完成信息的双向传输。分析一个通信协议，应当从其通信时序开始。

## 通信时序
![时序](https://imgs.raincorn.top/imgs/20200801162147.jpg)
* 起始位：数据传输的开始标志，电平由高到低
* 数据位：5~8位的逻辑0/1
* 校验位：用于校验数据传输的是否准确
* 停止位：数据传输的结束标志，电平由低变高
* 空闲位：当前无数据传输

## 波特率
波特率用于表示数据的传输速率，即每秒钟传输的bit数。一个完整的uart数据传输过程含有10个bit：起始位、数据位、终止位，例如常用的9600bps意为在1s内传输9600bit，对应960个uart包，数据96字节。

## 抓包
使用逻辑分析仪抓取TXD上的电平变化状态，以获取到MCU运行过程中发送数据时串口的时序变化与理论时序对比，下图演示的程序每1s向PC发送一次Hello World！字符串。

每1s发送一次字符串：

![](https://imgs.raincorn.top/imgs/20200801163423.png)

单次发送字符串时TXD电平的变化：

![](https://imgs.raincorn.top/imgs/20200801163937.png)

单个uart数据包上电平变化：

![](https://imgs.raincorn.top/imgs/20200801165537.png)

每10个位规定为一个uart数据包，起始位拉低电平，终止位拉高电平。数据位的传输过程中控制电平由低到高位传输数据，在上图中通过计算`0001 0010`对应的ASCII值即可获得对应发送的字符`H`。每10位为一包，依次分析单次发送中的电平变化即可或者该次发送的字符串数据。

# 配置

## 寄存器
在8051上使用串口通信，需要借助定时器，因此需要配置TMOD，SCON，AUXR（IAP15）寄存器。当然，可以借助STC-ISP生成。

* SCON寄存器，常用配置`SCON=0x50`，8位数据可变波特率。
![SCON](https://imgs.raincorn.top/imgs/20200801145839.png)

    * SM0 SM1 SM2：定义工作模式
    * REN：定义是否允许接收
    * TB8 RB8：发送与接收数据的第八位，使用校验位
    * TI RI：发送与接收数据完成时的中断标志位

* TMOD寄存器，常用配置`TMOD=0x20`，定时器1选择8位自动重装。
![TMOD](https://imgs.raincorn.top/imgs/20200728211832.png)

    * M0 M1：定义工作模式
    * C/T：定义计数/定时
    * GATE：选择定时器启动方式

* AUXR寄存器，常用配置`AUXR=0x00`，兼容传统8051。
![AUXR](https://imgs.raincorn.top/imgs/20200801172241.png)

## 波特率
波特率使用TH1 TL1来配置，在通信过程中需要保证双方的波特率相等，可直接参考STC-ISP软件生成的数据。

## SBUF
8051的串口中有两个寄存器，均为SBUF，一个是发送寄存器，一个是接收寄存器。二者在物理结构上独立，但名称均为SBUF，地址均为0x99。
* 发送时，`SBUF=dat`，此时代表将数据写入SBUF并由MCU发送。
* 接收时，`dat=SBUF`，此时代表是数据由SBUF写入。

## TI RI
* TI：发送标志位，当发送完成后置为1，需要手动复位位0；
* RI：接收标志位，当接收完成后置为1，需要手动复位为0。

# 实例
* 配置寄存器（使用STC-ISP生成）
```
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

```
* 中断处理
```
void Ser_Uart() interrupt 4
{
	if(RI) //如果完成接收数据则将RI置为0并将SBUF内容提取，将其加一后再发送给PC
	{
		RI=0;
        temp=SBUF;
		Send_Byte(temp+1);
	}
}
```
* 字节发送
```
void Send_Byte(uchar dat)
{
	SBUF=dat;
	while(TI==0);
	TI=0;
}
```

* 字符串发送
```
void Send_String(uchar *str)
{
	while(*str!='\0') 
	{
		SBUF=*str++;
		while(TI==0);
		TI=0;
	}
}
```

# 完整源码
功能：上电后立刻向PC机发送字符串`Hello World!`，PC机向单片机发送数据后单片机将数据加一后发送给PC机。

```
/*
2020-08-01 15：56
CT107D_IAP15F2K61S2
Baud：9600bps
*/
#include<stc15f2k60s2.h>
typedef unsigned char uchar;
typedef unsigned int uint;
void Send_Byte(uchar dat);
uchar temp;

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

void Delay1000ms()		//@11.0592MHz
{
	unsigned char i, j, k;

	i = 43;
	j = 6;
	k = 203;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
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
		temp=SBUF;
		Send_Byte(temp+1);
	}
}

void Send_Byte(uchar dat)
{
	SBUF=dat;
	while(TI==0);
	TI=0;
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
	Send_String("Hello World!");
	while(1)
	{
		
	}
}
```
