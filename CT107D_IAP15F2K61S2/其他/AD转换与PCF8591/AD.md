# A/D指标
* ADC位数：ADC输出数字量的总刻度，例如PCF8591的8位ADC，输出0~255一共256个刻度。
* 分辨率：意为ADC能够量化的最低输入变化率，例如当Vref=5v时，PCF8591的分辨率为0.02v。
* 基准：ADC转换过程中对比的基准数据，例如Vref=5v，当ADC输出为200时，待测模拟输入的电压为3.92v。
* 转换速率：在一秒内，ADC采样转化的次数。ADC主要分为三类：双积分型，转换需几百毫秒；逐次逼近型，转换需几十微秒；并行比较型，需几十纳秒（典型值）。

# 概述
* 8位逐次逼近A/D和D/A转换芯片，拥有4路模拟输出，1路数字输出
* 通过IIC总线与MCU通信
* 三个引脚用于地址信号编程（片选）

# 引脚
![](https://imgs.raincorn.top/imgs/20200807155102.png)
PCF8591拥有16个引脚，其中：
* AIN0~AIN3：四个模拟输入
* A0~A2：用于编程硬件地址（片选）
* VREF：基准电压的输入（CT107D上直接接到VCC，实测稍高于5V）
* AOUT：ADC时，模拟量输出的端口
* SCL，SDA：IIC总线接口，用于与单片机交换数据

# 编程
第一字节为器件地址字节：
![](https://imgs.raincorn.top/imgs/20200807161357.png)
在CT107D上，A0~A2均接地，又由于AD/DA的类型地址为1001，故其在IIC总线上的地址为：
* 读取：0x91
* 写入：0x90

第二字节为控制字节：
![](https://imgs.raincorn.top/imgs/20200807161456.png)
* 第7位：特征位，固定为0
* 第6位：模拟量输出控制，0不允许模拟量输出，1允许。
* 第5~4位：模拟量输入方式选择，00为四路单端输入，01为三路差分输入，10为二路单端一路差分，11为二路差分。
* 第3位：特征位，固定为0
* 第2位：自动增量控制，0禁止自动增量，1允许自动增量，即允许AIN端口在转换后自动加1
* 第1~0位：A/D通道选择，00为AIN0，01为AIN1等

在CT107D上：
AIN0连接到外接排针；AIN1连接到光敏电阻RD1；AIN2连接到放大器输出；AIN3连接到电位器Rb2。

# 实例
注意如需ADC，DAC一起工作，则需要将控制字节的第7为置为1，允许模拟量输出。
## ADC转换
```
uchar ADC_AIN(uchar ain)
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
```

## DAC转换
```
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
