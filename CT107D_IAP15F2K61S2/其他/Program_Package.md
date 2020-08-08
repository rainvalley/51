# Tips
* 善用位操作符，能够仅改变个别位并提高运算速度
* 减少人为造成死循环的使用
* 在时序要求严格的通信中（OneWire，超声波）应当关闭中断
* 数码管在调试时，如残影则应当增加切换时延时（为什么不用TM1640
* 在仿真过程中，避免操作S6，S7
* NE555测量中，P34需连接到SIGNAL，会干扰矩阵键盘

# 外设初始化

```
typedef unsigned char uchar;
typedef unsigned int uint;

void select(uchar channel)
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
		default:
			P2=(P2&0x1f);
			break;
	}
}

void init_sys()
{
	select(4);
	P0=0xff;
	select(5);
	P0=0x00;
	select(0);
}
```

# 中断

## 外部中断

```
void Init_INT0()
{
	EX0 = 1; //允许外部中断0，详见前文中断结构。
	IT0 = 1; //配置外部中断触发方式为下降沿
	EA = 1; //允许总中断
}

void Init_INT1()
{
	EX1 = 1;
	IT1 = 1;
	EA = 1;
}

void Ser_INT0() interrupt 0
{
	L1 = ~L1; //取反LED灯对应端口电平
}

void Ser_INT1() interrupt 2
{
	L2 = ~L2;
}
```

## 定时/计数

```
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
```

# 数码管

```
uchar code dig_code[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};
void display(uchar pos,uchar value)
{
	delay(1000);
	P0=0xff; //消隐
	select(6);
	P0=0x01<<pos; //位选
	select(7);
	P0=dig_code[value]; //段选
}
```

# 独立按键

```
void Timer0Init(void)		//5毫秒@11.0592MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0x00;		//设置定时初值
	TH0 = 0xEE;		//设置定时初值设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;
	EA = 1;
}

void Scan_Key_4(void)
{
	uint i;
	static uchar keybuff[] = {0xff,0xff,0xff,0xff};
	keybuff[0] = (keybuff[0]<<1)|S4;
	keybuff[1] = (keybuff[1]<<1)|S5;
	keybuff[2] = (keybuff[2]<<1)|S6;
	keybuff[3] = (keybuff[3]<<1)|S7;
	for(i = 0;i < 4;i ++)
	{
		if(keybuff[i] == 0xff) //连续扫描8次都是1，40ms内都是弹起状态，按键已松开
		{
			KeyStat[i] = 1;
		}
		else if(keybuff[i] == 0x00) //连续扫描8次都是0，40ms内都是按下状态，按键已按下
		{
			KeyStat[i] = 0;
		}
	}
}

void Long_Press() //长按功能
{
	if(KeyStat[2]==0)
	{
		count++;
	}
	else
	{
		count=0;
	}
	if(count==200)
	{
		flag=~flag;
		count=0;
	}
}

void Ser_Timer0() interrupt 1
{
	Scan_Key_4();
	Long_Press();
}
```

# 矩阵键盘

```
sbit KEY_IN_1 = P4^4;
sbit KEY_IN_2 = P4^2;
sbit KEY_IN_3 = P3^5;
sbit KEY_IN_4 = P3^4;
sbit KEY_OUT_1 = P3^0;
sbit KEY_OUT_2 = P3^1;
sbit KEY_OUT_3 = P3^2;
sbit KEY_OUT_4 = P3^3;
uchar KeySta[4][4]={{1,1,1,1},{1,1,1,1},{1,1,1,1},{1,1,1,1}};

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
	static uchar keyout = 0; //矩阵按键扫描输出行索引
	static uchar keybuff[4][4] = {{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff},{0xff,0xff,0xff,0xff}};	//矩阵按键扫描缓存区
	
	keybuff[keyout][0] = (keybuff[keyout][0] << 1) | KEY_IN_1; //将每一行的4个按键值移入缓存区
	keybuff[keyout][1] = (keybuff[keyout][1] << 1) | KEY_IN_2;
	keybuff[keyout][2] = (keybuff[keyout][2] << 1) | KEY_IN_3;
	keybuff[keyout][3] = (keybuff[keyout][3] << 1) | KEY_IN_4;
		
	//消抖后更新按键状态
	for(i = 0;i < 4;i ++)
	{
		if((keybuff[keyout][i] & 0x0f) == 0x00)
			KeySta[keyout][i] = 0; //连续4次扫描值都是0，即4×4ms内都是按下状态，认为按键已平稳按下
		else if((keybuff[keyout][i] & 0x0f) == 0x0f)
			KeySta[keyout][i] = 1; //连续4次扫描值都是1，即4×4ms内都是松开状态，认为按键已稳定弹起
	}
	
	//执行下一次的扫描输出
	keyout ++;
	keyout = keyout & 0x03; //索引加到4就归零
	switch(keyout) //根据索引，释放当前输出引脚，拉低下次的输出引脚
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
```

# 串口

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
```

# PWM

```
void init_timer0(void)		//100us@11.0592MHz
{
	AUXR &= 0x7F;		//定时器时钟12T模式
	TMOD &= 0xF0;		//设置定时器模式
	TL0 = 0xA4;		//设置定时初值
	TH0 = 0xFF;		//设置定时初值
	TF0 = 0;		//清除TF0标志
	TR0 = 1;		//定时器0开始计时
	ET0 = 1;
	EA = 1;
}

void ser_timer0() interrupt 1
{
	static uint s_count=0;
	s_count++;
	if(s_count==30)
	{
		L1=1;
	}
	if(s_count==100)
	{
        L1=0;
		s_count=0;
	}
}
```

# PCF8591

```
uchar ADC(uchar ain)
{
	uchar volta;
	IIC_Start();
	IIC_SendByte(0x90); //给PCF8591发送写控制字
	IIC_WaitAck();
	IIC_SendByte(ain+0x40); //给写入通道数，并允许模拟量输出
	IIC_WaitAck();
	IIC_Stop();
	
//读写方向发生变化，重新开始通信
	
	IIC_Start();
	IIC_SendByte(0x91); //给PCF8591发送读控制字
	IIC_WaitAck();
	volta=IIC_RecByte();
	IIC_SendAck(1);
	IIC_Stop();
	
	return volta;
}

void DAC(float volta)
{
	float dat = (volta/5.0)*255;
	IIC_Start();
	IIC_SendByte(0x90); //发送写控制字
	IIC_WaitAck();
	IIC_SendByte(0x40); //允许模拟量输出
	IIC_WaitAck();
	IIC_SendByte(dat); //发送需要转换的数字量
	IIC_WaitAck();
	IIC_Stop();
}
```

# AT24C02

```
void write_eeprom(uchar addr,uchar dat)
{
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck(); //发送控制字节
	IIC_SendByte(addr);
	IIC_WaitAck(); //发送需要写入的地址
	IIC_SendByte(dat);
	IIC_WaitAck(); //发送需要写入的数据
	IIC_Stop();
}

uchar read_eeprom(uchar addr)
{
	uchar dat;
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(addr);
	IIC_WaitAck();
	IIC_Stop(); //伪写入操作
	
	IIC_Start();
	IIC_SendByte(0xa1);
	IIC_WaitAck();
	dat=IIC_RecByte();
	IIC_SendAck(1); //发送非响应标志，停止接收数据
	IIC_Stop();
	return dat;
}
```

# DS18B20

```
uint get_temp()
{
	uchar LSB,MSB;
	uint temp=0;
	
	init_ds18b20();
	Write_DS18B20(0xcc); //跳过ROM匹配
	Write_DS18B20(0x44); //开始温度转换
	init_ds18b20();
	Write_DS18B20(0xcc); //跳过温度转换
	Write_DS18B20(0xbe); //读取温度
	
	LSB=Read_DS18B20();
	MSB=Read_DS18B20();
	init_ds18b20();
	temp=(MSB<<8)|LSB;
	temp=temp>>4; //只保留整数部分
	return temp;
}
```
# DS1302
```
uchar time[7]={0x45,0x58,0x23,0x01,0x01,0x03,0x20};
void init_ds1302()
{
	uchar address=0x80; //写地址
	uchar i;
	Write_Ds1302_Byte(0x8e,0x00); //关闭写保护
	for(i=0;i<7;i++) //轮流写入时间数据
	{
		Write_Ds1302_Byte(address,time[i]);
		address=address+2;
	}
	Write_Ds1302_Byte(0x8e,0x80);
}

void read_ds1302()
{
	uchar address=0x81; //读地址
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

# 超声波测距

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
```

# 红外接收

```
void init_int0()
{
	IR=1;
	EX0=1;
	IT0=1;
	EA=1;
}
//当接收到红外信号时，IR被置低电平，进入中断
void ser_int0() interrupt 0
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
}
```