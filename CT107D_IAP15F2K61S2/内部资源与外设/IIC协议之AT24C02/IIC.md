# 概述
* 由飞利浦开发的串行总线协议，在一个总线上仅有一个主器件，若干个从器件。
![](https://imgs.raincorn.top/imgs/20200807094553.png)
* IIC总线有两根数据线，数据线SDA，时钟线SCL。
* IIC总线上一个完整的器件地址包括类型划分符，片选信号与读写标志符。其中类型划分符用于划分不同器件的功能，例如EEPROM的类型地址为1010，AD/DA的类型地址为1001。片选信号用于在同类型中区分不同的芯片，读写标识符则用于标志读/写器件。
![](https://imgs.raincorn.top/imgs/20200807095147.png)

例如EEPROM器件片选信号0000，则：
* 读取设备的地址：0xA1
* 写入设备的地址：0xA0

# 起始/终止
**IIC总线上，SDA SCL默认上拉。在SCL高电平时，SDA上的数据变化会被判定为开始/结束标志。仅在SCL低电平时，允许SDA上数据的传输（电平变化）。**

![](https://imgs.raincorn.top/imgs/20200807095841.png)
* 起始信号：SCL高电平时，SDA拉低。
* 终止信号：SCL高电平时，SDA释放。

# 应答
应答信号用于数据接收，发送时的二次确认。主器件数据发送时，从器件产生应答；主器件数据接收时，主器件产生应答；主器件数据接收完成时，主器件产生非应答。

![](https://imgs.raincorn.top/imgs/20200807101124.png)
* 应答：SCL高电平时，SDA拉低。
* 非应答：SCL高电平时，SDA释放。

# 数据写入/读取
**数据交换时，SCL必须保证低电平，否则将会被判定为开始/结束标志。**
* 主器件写入，SCL低电平时，改变SDA上数据。
* 主器件读取，SCL高电平时，读取SDA上数据（从器件发送）。

# 通信流程
下示意图中，阴影部分为主器件发送，白色部分为从器件发送。S——开始，A——应答，P——停止。
* 主器件向从器件发送数据，先发送器件地址，再发送数据内容，从器件发送应答。

![](https://imgs.raincorn.top/imgs/20200807101712.png)

* 主器件从从器件读取数据，注意当数据接收完成后主器件需要发送**非应答**信号。

![](https://imgs.raincorn.top/imgs/20200807101757.png)

* 在通信过程中，如果主/从器件通行方向发生变化，则需要重新开始通信。

![](https://imgs.raincorn.top/imgs/20200807101908.png)

# 协议实现
* 起始
```
//总线启动条件，SCL为高时拉低SDA
void IIC_Start(void)
{
    SDA = 1;
    SCL = 1;
    IIC_Delay(DELAY_TIME);
    SDA = 0;
    IIC_Delay(DELAY_TIME);
    SCL = 0;	
}
```
* 终止
```
//总线停止条件，SCL为高时释放SDA
void IIC_Stop(void)
{
    SDA = 0;
    SCL = 1;
    IIC_Delay(DELAY_TIME);
    SDA = 1;
    IIC_Delay(DELAY_TIME);
}
```
* 等待应答
```
//等待应答，在SCL为高时读取SDA上的电平变化，为高时非应答，为低时应答
bit IIC_WaitAck(void)
{
    bit ackbit;
	
    SCL  = 1;
    IIC_Delay(DELAY_TIME);
    ackbit = SDA;
    SCL = 0;
    IIC_Delay(DELAY_TIME);
    return ackbit;
}
```
* 发送应答
```
//发送应答，在SCL为高时，由主机发送
void IIC_SendAck(bit ackbit)
{
    SCL = 0;
    SDA = ackbit;  					// 0：应答，1：非应答
    IIC_Delay(DELAY_TIME);
    SCL = 1;
    IIC_Delay(DELAY_TIME);
    SCL = 0; 
    SDA = 1;
    IIC_Delay(DELAY_TIME);
}
```
* 发送数据
```
//通过I2C总线发送数据，注意SCL高电平时不允许SDA数据变化
void IIC_SendByte(unsigned char byt)
{
    unsigned char i;

    for(i=0; i<8; i++)
    {
        SCL  = 0;
        IIC_Delay(DELAY_TIME);
        if(byt & 0x80) SDA  = 1;
        else SDA  = 0;
        IIC_Delay(DELAY_TIME);
        SCL = 1;
        byt <<= 1;
        IIC_Delay(DELAY_TIME);
    }
    SCL  = 0;  
}
```
* 接收数据
```
//从I2C总线上接收数据，在SCL高电平时读取到SDA数据
unsigned char IIC_RecByte(void)
{
    unsigned char i, da;
    for(i=0; i<8; i++)
    {   
    	SCL = 1;
        IIC_Delay(DELAY_TIME);
        da <<= 1;
        if(SDA) da |= 1;
        SCL = 0;
        IIC_Delay(DELAY_TIME);
    }
    return da;    
}

```

# 实例分析
对于AT24C02（2Kb）进行读写操作，注意AT24C02写入时需要延时（最大5ms），同时读取时需要进行伪操作：
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

//读写反向发生变化，重新开始通信。

	IIC_Start();
	IIC_SendByte(0xa1);
	IIC_WaitAck();
	dat=IIC_RecByte();
	IIC_SendAck(1); //发送非响应标志，停止接收数据
	IIC_Stop();
	return dat;
}
write_eeprom(0xff,0x05);
delay(1000);
temp=read_eeprom(0xff);
```
通信流程：

![](https://imgs.raincorn.top/imgs/20200807123350.png)
* 协议起始，发送控制字节0xA0，写入地址0xFF，写入数据0x05，协议终止。
![](https://imgs.raincorn.top/imgs/20200807114554.png)

* 协议起始，发送控制字节0xA0，写入地址0xFF，协议终止；协议起始，发送控制字节0xA1，从从器件读取数据0x05，主器件发送非应答信号，协议终止。
![](https://imgs.raincorn.top/imgs/20200807114623.png)

