定时器与计数器本质上均为计数器，通过计数器的不断累加最后溢出向CPU提出中断，进入中断服务函数。

# 定时器原理
>计数脉冲来源选择外部晶振的12分频，16位定时器，晶振12MHz，充当定时器。

系统时钟为12MHz，12分频后的定时器时钟为1MHz，单个计时脉冲对应1us。16位定时器（TLx THx）最大值为65535，每次递增均需要1us，因此定时器最大定时时间为65535us。假设定时50ms，则需要预留50000个脉冲间隔，即：
```
TH0 = (65535-50000)/256;
TL0 = (65535-50000)%256;
```
>计数脉冲来自T0，T1引脚对应的外部脉冲信号时，充当计数器。

# 配置
在定时器属性的配置中，一般拥有如下步骤：
* 配置工作模式
控制定时器工作模式的寄存器为TMOD（仅字节寻址），具体参照下图：

    ![](https://imgs.raincorn.top/imgs/20200728211832.png)

    例如配置定时/计数器0，8位自动重装计数功能；定时/计数器1，16位定时功能：TMOD=0X16。
* 配置定时器初始赋值
根据需要定时的值来计算初始赋值，如定时50ms需要配置：
    ```
    TH1 = (65535-50000)/256;
    TL1 = (65535-50000)%256;
    ```
* 打开中断与启用定时器
    ```
    ET1 = 1;
    EA = 1;
    TR1 = 1;
    ```
* 配置中断服务函数：对初始赋值进行重装，同时注意避免服务函数中出现复杂的业务逻辑。

# 应用
>板上NE555连接到P34，对应T0引脚，测量NE555频率（电位器RB3控制）并根据频率点亮L1~L4。

具体实现：
```
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

int main()
{
	Init_Sys();
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
```

# 效果
<iframe src="//player.bilibili.com/player.html?aid=286598583&bvid=BV1Af4y1974G&cid=217417755&page=1" scrolling="no" border="0" frameborder="no" framespacing="0" allowfullscreen="true"> </iframe>