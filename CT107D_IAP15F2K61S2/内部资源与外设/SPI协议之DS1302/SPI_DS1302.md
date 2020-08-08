# 概述
* 低功耗实时时钟，2.0V时电流小于300nA，可使用备用电源
* 三线非标准SPI接口与MCU通信

DS1302典型电路：

![典型电路](https://imgs.raincorn.top/imgs/20200805171445.png)

DS1302引脚定义：

![引脚定义](https://imgs.raincorn.top/imgs/20200805171542.png)

# Command格式
DS1302在写入时，需要先写入命令字节，再写入具体数据，即满足格式`Write_DS1302(uchar command,uchar dat)`。

![Command](https://imgs.raincorn.top/imgs/20200805171931.png)
# RTC寄存器
此处RTC寄存器的定义是由Command格式而来，例如写秒寄存器（80H）：Command字节从高位到低位以此为`1000 0000`。
* 对应BIT7为1，允许写入DS1302；
* A0-A4均为0；
* BIT0为0，表示写操作。

![RTC](https://imgs.raincorn.top/imgs/20200805171914.png)

# BURST模式
使用传统的逐字节读取寄存器模式来获取数据，各个寄存器的数据读取存在时间差，可能会在进位的时候出现偏差。例如在读取`23:59:59`时，由于从秒开始读，可能会存在读取到分钟的时候已经发生了进位，即读取到`00:00:59`。

使用BRUST模式读取数据，能够一次性将寄存器数据读取到缓冲区，再供单片机一次性读取。如需使用BRUST模式，A0-A4地址全部置为1，需要向0xbe写入7个字节，从0xbf读取7个字节。

![](https://imgs.raincorn.top/imgs/20200805175720.png)

# 通信时序
**对DS1302，上升沿采样，下降沿输出。**

DS1302的通信协议为变种三线SPI，与标准SPI协议略有不同。注意：
* 在读取/写入时CE必须处于高电平状态
* 由于STC上无SPI接口，因此使用模拟SPI方式，注意需要在SCLK电平拉高前就写入数据，否则可能会出错。
* SCLK时序在读写时不变，只需要修改SDA上的数据即可完成读写。

>单字节上升沿写入

![](https://imgs.raincorn.top/imgs/20200805174820.png)
* SCLK低电平时，拉高CE（RST）端电平
* 在上升沿时，DS1302从I/O（SDA）读取到数据
* 单字节写入时需要两个字节，即控制字节与具体数据

>单字节下降沿读取

![](https://imgs.raincorn.top/imgs/20200805174848.png)
* SCLK低电平时，拉高CE（RST）端电平
* 在下降沿时，MCU从I/O（SDA）读取到数据
* 单字节读取需要写入一个控制字节，紧接着从DS1302读取到返回的数据

# BCD
在DS1302的读取写入中，均使用BCD码，即使用16进制表达十进制。例如0x12，在16进制表达中为18（十进制），在BCD码表达中为12（十进制）。

如需将BCD转为十进制，只需要对16除/取整：
```
uchar bcd_to_num(uchar bcd)
{
    uchar a,b;
    a = bcd / 16;
    b = bcd % 16;
    return (10*a+b);    
}
```

如需将十进制转化为BCD码，只需要对16做乘法：
```
uchar num_to_bcd(uchar num)
{
    uchar a,b;
    a = num / 10;
    b = num % 10;
    return (16*a+b);
}
```

# 实例
>底层驱动
```
#include <reg52.h>
#include <intrins.h>

sbit SCK = P1 ^ 7; //时钟线
sbit SDA = P2 ^ 3; //数据线
sbit RST = P1 ^ 3; //DS1302使能引脚
void Write_Ds1302(unsigned char temp) //DS1302写函数
{
	unsigned char i;
	for (i = 0; i < 8; i++)
	{
		SCK = 0; //拉低SCK准备实现上升沿
		SDA = temp & 0x01; //取temp的最低位，每次发送一位
		temp >>= 1; //将temp右移，清除刚刚发送的一位
		SCK = 1; //将数据准备好，在上升沿时，SDA的数据写入DS1302
	}
}

void Write_Ds1302_Byte(unsigned char address, unsigned char dat) //向特定地址写入数据
{
	RST = 0; 
	_nop_();
	SCK = 0;
	_nop_();
	RST = 1; 
	_nop_(); //SCK低电平时拉高RST电平
	Write_Ds1302(address); //写入地址
	Write_Ds1302(dat); //写入数据
	RST = 0; //拉低RST电平复位
}

unsigned char Read_Ds1302_Byte(unsigned char address)
{
	unsigned char i, temp = 0x00;
	RST = 0;
	_nop_();
	SCK = 0;
	_nop_();
	RST = 1;
	_nop_(); //SCK低电平时拉到RST电平
	Write_Ds1302(address); //控制字节完成后，SCK=1
	for (i = 0; i < 8; i++)
	{
		SCK = 0; //拉低SCK后，数据被DS1302写到SDA
		temp >>= 1;
		if (SDA)
			temp |= 0x80;
		SCK = 1; 
	}
	RST = 0;
	_nop_();
	SCK = 0;
	_nop_();
	SCK = 1;
	_nop_();
	SDA = 0;
	_nop_();
	SDA = 1;
	_nop_();
	return (temp);
}
```

>时钟实现

```
void init_ds1302()
{
	uchar address=0x80; //秒寄存器写入
	uchar i;
	Write_Ds1302_Byte(0x8e,0x00); //允许读写
	for(i=0;i<7;i++)
	{
		Write_Ds1302_Byte(address,time[i]); //向address写入time数组
		address=address+2; //地址加2，开始写入下一寄存器
	}
	Write_Ds1302_Byte(0x8e,0x80);
}

void read_ds1302()
{
	uchar address=0x81; //秒寄存器读取
	uchar i;
	for(i=0;i<7;i++)
	{
		time[i]=Read_Ds1302_Byte(address);
		address=address+2;
	}
}

void display_time()
{
	read_ds1302();
	display(0,time[2]/16);
	display(1,time[2]%16);
	display(2,10);
	display(3,time[1]/16);
	display(4,time[1]%16);
	display(5,10);
	display(6,time[0]/16);
	display(7,time[0]%16);
}

```

# 逻辑分析
>写入
`Write_Ds1302_Byte(0x8e,0x00);`：
* SCK低电平时将RST拉高，才能开始读写DS1302。
* SCK时序保持一定，每次在上升沿时DS1302**采样**一次，即MCU写入。
* 单字节写入需要写入控制字节与内容，`Write_Ds1302_Byte(0x8e,0x00);`意为向0x8e写入0x00，即允许写入。
![](https://imgs.raincorn.top/imgs/20200805181808.png)
>读取
`Read_Ds1302_Byte(0x81);`：
* SCK低电平时将RST拉高，才能开始读写DS1302。
* SCK时序保持一定，每次在下降沿时DS1302**输出**一次，即MCU读取。
* 单字节读取需要写入一个控制字节并读取一个字节数据。
![](https://imgs.raincorn.top/imgs/20200805182335.png)
