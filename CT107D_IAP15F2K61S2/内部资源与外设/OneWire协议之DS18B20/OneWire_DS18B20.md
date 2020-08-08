# 概述
* 工作电压3.0V~5.5V，工作温度-55°~+125°
* 分辨率9~12位，默认12位，分辨率为0.0625
* 通过单总线与MCU通信，接上拉电阻，默认为高电平

# 温度储存格式

![](https://imgs.raincorn.top/imgs/20200806124056.png)

* DS18B20以16位带符号位扩展的二进制补码形式读出。
* 高5位（BIT15~BIT11）为符号部分，当为0时表示正数，为1时表示负数。
* 中间7位（BIT10~BIT4）为整数部分，以2为位权。
* 低4位（BIT3~BIT0）为小数部分，默认分辨率0.0625。
* 如果只需要整数部分，则`temp=(MSB<<8)|LSB`，`temp=temp>>4`。
* 如果需要读取小数部分，小数部分需要乘以分辨率0.0625。

实际温度与二进制输出如下图（上电默认+85°）：

![](https://imgs.raincorn.top/imgs/20200806124920.png)

# 内部储存
DS18B20内部有64位的ROM，9字节高速SCRATCHPAD，3字节EEPROM。其中64位ROM用于储存OneWire总线上设备的唯一ID号，SCRATCHPAD用于暂存数据，EEPROM则用于掉电重加载数据。

>SCRATCHPAD分配

![](https://imgs.raincorn.top/imgs/20200806125158.png)
* 第0字节：温度数据低八位
* 第1字节：温度数据高八位
* 第2字节：TH配置字节，配置温度触发上限，上电后从EEPROM读取
* 第3字节：TL配置字节，配置温度触发下限，上电后从EEPROM读取
* 第4字节：配置寄存器，用于配置转换精度，上电后从EEPROM读取
* 第5字节~第7字节：预留，无用处
* 第8字节：CRC校验码

# 通信时序
DS18B20的通信分为三步：复位，写入，读取。注意，由于单总线对时序要求严格，所以尽量在通信时关闭中断。
## 复位
![](https://imgs.raincorn.top/imgs/20200806125945.png)
* MCU将总线拉低480us以上，然后释放总线
* 释放总线后，会由上拉电阻拉至高电平
* DS18B20读取到复位信号后等待15~60us，拉低总线，表示应答
* MCU可以在DS18B20应答后读取总线上的电平，如果为低电平则表示成功复位
* DS18B20产生60~240us应答信号后，会释放总线至高电平

```
//DS18B20设备初始化
bit init_ds18b20(void)
{
  	bit initflag = 0;
  	DQ = 1;
  	Delay_OneWire(12);
  	DQ = 0;
  	Delay_OneWire(80);
  	DQ = 1; //拉低总线480us以上，然后释放
  	Delay_OneWire(10); 
    initflag = DQ; //等待15-60us后，将DQ写入initflag，若为低则DS18B20成功应答
  	Delay_OneWire(5);
  	return initflag;
}

```

## 写入
![](https://imgs.raincorn.top/imgs/20200806130555.png)
* MCU将总线拉低约15us
* 在接下来的15-45us内，根据需要发送的数据，向总线上写入1/0
* 发送完数据后，释放总线

```
//通过单总线向DS18B20写一个字节
void Write_DS18B20(unsigned char dat)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		DQ = 0; //拉低总线10-15us
		DQ = dat&0x01; //向总线写入数据
		Delay_OneWire(5); //维持数据15-45us 
		DQ = 1; //释放总线
		dat >>= 1; //准备发送下一个数据位
	}
	Delay_OneWire(5);
}
```

## 读取
![](https://imgs.raincorn.top/imgs/20200806130808.png)
* MCU将总线拉低1~15us
* 拉低引脚后，MCU读取总线上的电平数据
* 读取总线数据后，延时45us，等待释放总线

```
//从DS18B20读取一个字节
unsigned char Read_DS18B20(void)
{
	unsigned char i;
	unsigned char dat;
  
	for(i=0;i<8;i++)
	{
		DQ = 0; //拉低总线10-15us
		dat >>= 1;
		DQ = 1;
		if(DQ)
		{
			dat |= 0x80;
		}	    
		Delay_OneWire(5); //读取总线电平后延迟45us
	}
	return dat;
}
```

# 实例
```
uint get_temp()
{
	EA=0;
	uchar LSB,MSB;
	uint temp=0;
	
	init_ds18b20(); //复位DS18B20
	Write_DS18B20(0xcc); //跳过ROM
	Write_DS18B20(0x44); //开始温度转换
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0xbe); //读取高速暂存器数据
	LSB=Read_DS18B20();
	MSB=Read_DS18B20();
	init_ds18b20(); //读取字节0和1后，复位DS18B20，表示读取结束
	temp=(MSB<<8)|LSB;
	temp=temp>>4; //移除小数部分，保留整数部分
	EA=1；
	return temp;
}

void display_ds18b20()
{	
	uint temp=get_temp();
	display(7,temp%10);
	display(6,temp/10);
}

```