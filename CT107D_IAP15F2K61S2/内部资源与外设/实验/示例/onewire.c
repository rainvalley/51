/*
  程序说明: 单总线驱动程序
  软件环境: Keil uVision 4.10 
  硬件环境: CT107单片机综合实训平台(外部晶振12MHz) STC89C52RC单片机
  日    期: 2011-8-9
  修改时间：2020-1-19
  修改环境：IAP15F2K61S2
*/
#include "reg52.h"
#include "onewire.h"

sbit DQ = P1^4;  //单总线接口

//单总线延时函数
void Delay_OneWire(unsigned int t)  //STC89C52RC
{
	t=t*12;//增加12延迟
	while(t--);
}

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






