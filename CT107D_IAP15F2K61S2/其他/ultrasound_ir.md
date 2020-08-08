在蓝桥杯CT107D上，J2用于超声波/红外选择，其中TX连接到P1^0，RX连接到P1^1。其中，`1-3 2-4`用于选择到超声波，`5-3 6-4`选择到红外。

![](https://imgs.raincorn.top/imgs/20200808173228.png)

# 超声波

## 原理
超声波发射模块发射超声波，在发送的同时开始计时，当接收端接收到超声波时，停止计时。由于超声波在空气中传播的速率固定，因此可以根据传播时间字节计算距离。假定室温20°，距离S = 344*T/2 (m)。

## 步骤
* 插针选择到`1-3 2-4`
* TX引脚上发射模块发射9个40KHz的超声波信号
* 在发射的同时，启动定时器，开始计时
* 当超声波遇到障碍物后，会被反射回接收端RX，使拉低RX电平
* 接收端接收到反射波时，停止计时，此时统计整个过程的时间
* 根据S = V*T/2(m)计算单向的距离
* 注意发送间隔最好大于100ms，避免冲突
* 注意需要关闭定时器中断，避免溢出后会卡死

## 实例
```
void send_wave()
{
	uchar i;
	for(i=0;i<8;i++) //发送8个40KHz的超声波
	{
		TX=1;
		delay12us();
		TX=0;
		delay12us();
	}
}

void measure_distance()
{
	uint time=0;
	TMOD|=0x10;
	TL1=0;
	TH1=0;
	TF1=0;
	ET1=0;
	
	send_wave(); //开始发送并启动定时器统计接收的时间
	TR1=1;
	while((RX==1)&&(TF1==0)); //当未接收到反射且定时器未溢出时，死循环
	TR1=0;
	
	if(TF1==0) //定时器未溢出，根据反射时间计算举例
	{
		time=(TH1<<8)|TL1;
		distance=((time/10)*17)/100+3;
	}
	else //定时器溢出，超过最大量程
	{
		TF1=0;
		distance=999;
	}
}

void display_distance()
{
	if(distance==999)
	{
		display(0,10);
	}
	else
	{
		display(0,distance/100);
		display(1,distance%100/10);
		display(2,distance%10);
	}
}

void init_timer0(void)		//50毫秒@11.0592MHz
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

void ser_timer0() interrupt 1
{
	static uchar count=0;
	if(count==4)
	{
		count=0;
		measure_distance();
	}
	count++;
}	
```

# 红外
CT107D上有发送与接收模块，使用前需要连接`5-3 4-6`引脚。在红外中，载波部分为低电平，闲置部分为低电平。常用NEC通信协议，整个过程分为引导码，用户码，数据码。

![](https://imgs.raincorn.top/imgs/20200808175319.jpg)

## 0/1
上图中，Logic 1为逻辑高电平，Logic 0为逻辑低电平。其中，阴影部分为载波低电平，空白部分为高电平。
* 对于逻辑1：低电平保持560us，高电平保持2.25ms-560us。
* 对于逻辑0：低电平保持560us，高电平保持1.12ms-560us。

## 引导码
在NEC标准中，引导码用于标志通信的起始。对应上图中9ms的低电平，4.5ms的高电平。

## 用户码，数据码
用户码用于区分不同设备，避免不同红外遥控器之间的干扰。在通信的过程中，为了避免误码，需要对用户码与用户反码进行判定。

## 实例
```
void display_ir()
{
	display(0,irdata[4]/100);
	display(1,irdata[4]%100/10);
	display(2,irdata[4]%10);
	
	display(5,irdata[5]/100);
	display(6,irdata[5]%100/10);
	display(7,irdata[5]%10);
}

//当接收到红外信号时，IR被置低电平，进入中断
void ser_int0() interrupt 1
{
	uchar i,j,n=0;
	delay(2000);
	if(IR==1) //判断是否为干扰
	{
		return;
	}
	while(!IR); //引导码载波9ms，低电平
	while(IR);	//引导码空闲4.5ms，高电平
	
	for(j=0;j<4;j++)
	{
		for(i=0;i<4;i++)
		{
			while(!IR); //数据0/1均拥有相同时间的载波，低电平			
			while(IR)
			{
				delay140us(); //每140us将计数值加一，统计空闲时间
				n++;
				if(n>=30)
				{
					return;
				}
			}
			irdata[j]=irdata[j]>>1;
			if(n>=8) //当空闲时间大于1120us时，判定数据为1
			{
				irdata[j]|=0x80;
			}
			n=0;
		}
	}
    if(irdata[0]==~irdata[1]) //用户码与用户反码匹配
    {
        irdata[4]=irdata[0];
    }
    if(irdata[2]==~irdata[3]) //数据码与数据反码匹配
    {
        irdata[5]=irdata[2];
    }
}
```