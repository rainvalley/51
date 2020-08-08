#include<reg52.h>
#include"onewire.h"
#include"ds1302.h"
#include"data.h"
#include"iic.h"
#include"intrins.h"
void Display_Dynamic();
uchar code dig_code[10]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90};//数码管显示内容
uchar time[7]={0x50,0x59,0x23,0x01,0x01,0x01,0x20};//DS1302初始化内容
uint temp=0;
int data_pcf8591=0;
uint data_at24c02=0;
uint distance=0;
sbit TX=P1^0;
sbit RX=P1^1;
uint j;
uint count_times=0;
uint count_time=0;
uint frequency=0;
//延时函数
void Delay(uint t)
{
	while(t--);
}

//选择138译码器输出端
void selectHC573(uchar channel)
{
	switch(channel)
	{
		case 4:
		P2=(P2&0x1f)|0x80;//不改变P2其他端口电平的情况下改变138译码器输入，由于前移一位故channel*2。
		break;
		case 5:
		P2=(P2&0x1f)|0xa0;
		break;
		case 6:
		P2=(P2&0x1f)|0xc0;
		break;
		case 7:
		P2=(P2&0x1f)|0xe0;
	}
}

//按位点亮数码管
void DisplaySMG_Bit(uchar value,uchar pos)
{	
	P0=0XFF;
	selectHC573(6);
	P0=0X01<<pos;//channel选择为6时选择数码管位置
	selectHC573(7);
	P0=dig_code[value];//channel选择为7时选择数码管显示内容
}

//获取温度
void tempget()
{
	uchar low,high;
	EA=0;
	init_ds18b20();
	Write_DS18B20(0XCC);
	Write_DS18B20(0X44);

	init_ds18b20();
	Write_DS18B20(0XCC);
	Write_DS18B20(0Xbe);
/*
DS18B20操作步骤(P11)：
1-初始化
2-跳过ROM
3-转换温度(0x44)/开始读取温度Scratchpad(0xbe)
*/

	low=Read_DS18B20();
	high=Read_DS18B20();

	temp=high<<4;
	temp|=(low>>4);//将DS18B20数据转换为十进制并截取整数部分
	EA=1;
} 

//动态显示温度
void Display_Dynmaic_temp()
{
	tempget();
	DisplaySMG_Bit(temp%100/10,5);
	Delay(500);
	P0=0XFF;
	DisplaySMG_Bit(temp%10,6);
	Delay(500);
	P0=0XFF;
	//显示温度整数部分
	selectHC573(6);
	P0=0X01<<7;
	selectHC573(7);
	P0=0XC6;//显示C代表摄氏度
}
									  
//DS1302初始化

void ds1302_init()
{
	uchar add=0x80;//写数据地址
	uchar i;
	Write_Ds1302_Byte(0x8e,0x00); //关闭写保护
	for(i=0;i<7;i++)
	{
		Write_Ds1302_Byte(add,time[i]);
		add=add+2;
	}
	Write_Ds1302_Byte(0x8e,0x80); //开启写保护
}

//读取DS1302时间，地址可见P9，WP位为写保护控制位
void ds1302_read()
{
	uchar i;
	uchar add=0x81;//读数据地址
	Write_Ds1302_Byte(0x8e,0x00);//关闭写保护
	for(i=0;i<7;i++)
	{
		time[i]=Read_Ds1302_Byte(add);
		add=add+2;
	}
	Write_Ds1302_Byte(0x8e,0x80);//开启写保护
}

//动态显示时间
void Display_Dynmaic_time()
{
	ds1302_read();
	DisplaySMG_Bit(time[1]/16,0); //BCD码取十位
	Delay(500);
	P0=0XFF;
	DisplaySMG_Bit(time[1]%16,1);//BCD码取个位
	Delay(500);
	P0=0XFF; //以上是分钟的显示
	selectHC573(6);
	P0=0X01<<2;
	selectHC573(7);
	P0=~0X40;
	Delay(500);
	P0=0XFF; //以上是分隔符显示
	DisplaySMG_Bit(time[0]/16,3);
	Delay(500);
	P0=0XFF;
	DisplaySMG_Bit(time[0]%16,4);	//以上是秒的显示
}

//从A/D通道3读取数据
void Read_AIN3()
{
	IIC_Start();		//IIC总线起始信号							
	IIC_SendByte(0x90); 	//PCF8591的写设备地址		
	IIC_WaitAck();  	//等待从机应答
			
	IIC_SendByte(0x01); 	//写入PCF8591的控制字节		
	IIC_WaitAck();  	//等待从机应答						
	IIC_Stop(); 		//IIC总线停止信号					
	


	IIC_Start();											
	IIC_SendByte(0x91); 		
	IIC_WaitAck();
	 				
	data_pcf8591 = IIC_RecByte();			
	IIC_SendAck(1); 		//产生非应答信号				
	IIC_Stop(); 		//IIC总线停止信号					
}


void Display_Dynmaic_pcf8591()
{
	Read_AIN3();
   	DisplaySMG_Bit(data_pcf8591/100,0);
	Delay(500);
	DisplaySMG_Bit((data_pcf8591%100)/10,1);
	Delay(500);
	DisplaySMG_Bit(data_pcf8591%10,2);
	Delay(500);
}

void write_at24c02(uchar addr,uint data_at24c02)
{
	IIC_Start();
	IIC_SendByte(0XA0);
	IIC_WaitAck();
	IIC_SendByte(addr);
	IIC_WaitAck();
	IIC_SendByte(data_at24c02);
	IIC_WaitAck();
	IIC_Stop();
}

uint read_at24c02(uchar addr)
{
	uchar tmp_at24c02;
	IIC_Start();
	IIC_SendByte(0XA0);
	IIC_WaitAck();
	IIC_SendByte(addr);
	IIC_WaitAck();

	IIC_Start();
	IIC_SendByte(0XA1);
	IIC_WaitAck();
	tmp_at24c02=IIC_RecByte();
	IIC_SendAck(1);
	IIC_Stop();
	return tmp_at24c02;
}

void Display_Dynmaic_at24c02()
{	
	write_at24c02(0X01,0x0f);
	data_at24c02=read_at24c02(0x01);
	DisplaySMG_Bit(data_at24c02/100,0);
	Delay(500);
	DisplaySMG_Bit(data_at24c02%100/10,1);
	Delay(500);
	DisplaySMG_Bit(data_at24c02%10,2);
	Delay(500);
	
}

void Delay12us()		//@12.000MHz
{
	unsigned char i;

	_nop_();
	_nop_();
	i = 33;
	while (--i);
}

void send_wave()
{
	uchar i;
	for(i=0;i<8;i++)
	{
		TX=1;
		Delay12us();
		TX=0;
		Delay12us();
	}
}

void measure_distance()
{
	uint time_sonic=0;
	TMOD&=0X0F;
	TL1=0X00;
	TH1=0X00;

	send_wave();
	TR1=1;
	while((RX==1)&&(TF1==0));
	TR1=0;

	if(TF1==0)
	{
		time_sonic=TH1;
		time_sonic=(time_sonic<<8)|TL1;
		distance=((time_sonic/10)*17)/100+3;
	}
	else
	{
		TF1=0;
		distance=999;
	}
}

void Display_Dynmaic_sonic()
{
	measure_distance();
	for(j=0;j<100;j++)
	{
		DisplaySMG_Bit(distance/100,0);
		Delay(500);
		DisplaySMG_Bit(distance%100/10,1);
		Delay(500);
		DisplaySMG_Bit(distance%10,2);
		Delay(500);
	}
	
}

void init_timer()
{
	TH0=0XFF;
	TL0=0XFF;
	TH1=(65536-50000)/256;
	TL1=(65536-50000)%256;

	TMOD=0X16;

	ET1=1;
	ET0=1;
	EA=1;
	TR0=1;
	TR1=1;
}

void t0_service() interrupt 1
{
	count_times++;
}

void t1_service() interrupt 3
{
	TH1=(65536-50000)/256;
	TL1=(65536-50000)%256;
	count_time++;
	if(count_time==20)
	{		   
		frequency=count_times;
		count_times=0;
		count_time=0;
	}
}

void Display_Dynmaic_ne555()
{
	DisplaySMG_Bit(frequency/10000,3);
	Delay(500);
	DisplaySMG_Bit(frequency/1000%10,4);
	Delay(500);
	DisplaySMG_Bit(frequency/100%10,5);
	Delay(500);
	DisplaySMG_Bit(frequency/10%10,6);
	Delay(500);
	DisplaySMG_Bit(frequency%10,7);
	Delay(500);
}

int main()
{
	selectHC573(5);
	P0=0X00;//初始化板上资源，关闭蜂鸣器与继电器
	selectHC573(7);
	ds1302_init();
	init_timer();
	while(1)
	{	
		//Display_Dynmaic_temp();
		Display_Dynmaic_time();
		//Display_Dynmaic_pcf8591();
		//Display_Dynmaic_at24c02();
		//Display_Dynmaic_sonic();
		//Display_Dynmaic_ne555();
	}
}