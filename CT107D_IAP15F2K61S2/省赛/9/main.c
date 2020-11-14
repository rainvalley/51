#include<stc15f2k60s2.h>
#include"iic.h"
#include<intrins.h>
typedef unsigned char uchar;
typedef unsigned int uint;
uchar code dig_code[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf,0xff};
//����״̬�룬����״̬���ݣ�Ĭ����תʱ��
uchar keystat[4]={1,1,1,1};
uchar keybackup[4]={1,1,1,1};
uchar time[4]={4,4,4,4};
//��������Ϊ��800ms��˸��־λ��ģʽ��־λ����ѡ���ģʽ��־λ��pwmռ�ձȣ�LED���е�ģʽ��LED����ʱP0��Ӧֵ��at24c02��д��д��־��
uchar flag_800ms=0,mode=0,select_mode=1,pwm=25,stat=0,data_led=0,flag_write=0;
sbit S4 = P3^3;
sbit S5 = P3^2;
sbit S6 = P3^1;
sbit S7 = P3^0;
void set_pwm();
uchar read_at24c02(uchar address);
void display_data();
void led_run();
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
	time[0]=read_at24c02(0x00);
	time[1]=read_at24c02(0x01);
	time[2]=read_at24c02(0x02);
	time[3]=read_at24c02(0x03);
}

void delay(uint t)
{
	while(t--);
}

void display(uchar pos,uchar num)
{
	select(6);
	P0=0x01<<pos;
	select(7);
	P0=dig_code[num];
	delay(1500);
	P0=0xff;
	select(0);
}

uchar read_pcf8591(uchar ain)
{
	uchar volta;
	IIC_Start();
	IIC_SendByte(0x90);
	IIC_WaitAck();
	IIC_SendByte(ain);
	IIC_WaitAck();
	IIC_Stop();
	
	IIC_Start();
	IIC_SendByte(0x91);
	IIC_WaitAck();
	volta=IIC_RecByte();
	IIC_Ack(0);
	IIC_Stop();
	
	return volta;
}

void write_at24c02(uchar address,uchar dat)
{
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(address);
	IIC_WaitAck();
	IIC_SendByte(dat);
	IIC_WaitAck();
	IIC_Stop();
}

uchar read_at24c02(uchar address)
{
	uchar dat;
	IIC_Start();
	IIC_SendByte(0xa0);
	IIC_WaitAck();
	IIC_SendByte(address);
	IIC_WaitAck();
	IIC_Stop();
	
	IIC_Start();
	IIC_SendByte(0xa1);
	IIC_WaitAck();
	dat=IIC_RecByte();
	IIC_Ack(0);
	IIC_Stop();
	
	return dat;
}


void scan_key()
{
	uchar i;
	static uchar keybuff[4]={0xff,0xff,0xff,0xff};
	keybuff[0]=(keybuff[0]<<1)|S4;
	keybuff[1]=(keybuff[1]<<1)|S5;
	keybuff[2]=(keybuff[2]<<1)|S6;
	keybuff[3]=(keybuff[3]<<1)|S7;
	for(i=0;i<4;i++)
	{
		if(keybuff[i]==0xff)
		{
			keystat[i]=1;
		}
		if(keybuff[i]==0x00)
		{
			keystat[i]=0;
		}
	}
}


void init_timer0(void)		//1����@11.0592MHz
{
	AUXR |= 0x80;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TL0 = 0xCD;		//���ö�ʱ��ֵ
	TH0 = 0xD4;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
	ET0 = 1;
	EA = 1;
}

void ser_timer0() interrupt 1
{
	static uchar update_count=0;
	static uint display_count=0;
	display_count++;
	update_count++;
	scan_key(); //ÿ1msɨ��һ�μ���
	if(display_count==800) //ÿ800ms��˸һ�������
	{
		flag_800ms=~flag_800ms;
		display_count=0;
	}
	if(update_count==100) //ÿ100ms����һ��at24c02��pcf8591
	{
		flag_write=~flag_write;
		update_count=0;
		set_pwm(); //ÿ100ms����һ��pwmռ�ձ�
	}
}

void key_fun(uchar key)
{
	if(key==3) //ֹͣTR1�ļ���
	{
		if(TR1==0)
		{
			TR1=1;
		}
		else
		{
			TR1=0;
		}
			
	}
	if(key==2) //�ı�����ܹ���ģʽ��0����Ϩ��1��������ģʽ��2������ת���
	{
		mode++;
		if(mode==3)
		{
			mode=0;
		}
	}
	if(key==1&&mode==1) //��������ģʽ
	{
		select_mode++;
		if(select_mode==5)
		{
			select_mode=1;
		}
	}
	if(key==0&&mode==1) //��������ģʽ
	{
		select_mode--;
		if(select_mode==0)
		{
			select_mode=4;
		}
	}
	
	if(key==1&&mode==2) //������ת���
	{
		time[select_mode-1]++;
		if(time[select_mode-1]>=13)
		{
			time[select_mode-1]=4;
		}
	}
	if(key==0&&mode==2) //������ת���
	{
		time[select_mode-1]--;
		if(time[select_mode-1]<=3)
		{
			time[select_mode-1]=12;
		}
	}
}

void key_press()
{
	int i;
	for(i=0;i<4;i++)
	{
		if(keystat[i]!=keybackup[i])
		{
			if(keybackup[i]!=0)
			{
				key_fun(i);
			}
			keybackup[i]=keystat[i];
		}
	}
}

void display_data()
{
	if(mode==0) //�������ܴ���Ϩ��״̬��������ʾ����״̬
	{
		if(keystat[0]==0&&mode==0)
		{
			while(keystat[0]==0)
			{
				display(6,10);
				display(7,pwm/25);
			}
		}
		else
		{
			select(6);
			P0=0xff;
			select(7);
			P0=0xff;
		}
	}
	if(mode==1) //���ѡ������ģʽ
	{
		if(flag_800ms) //800ms��˸�����
		{
			display(0,10);
			display(1,select_mode);
			display(2,10);
		}
		else
		{
			display(0,11);
			display(1,11);
			display(2,11);
		}
		if(time[select_mode-1]>=10) //��ת���>=1000ʱ��ʾ����λ
		{
			display(4,time[select_mode-1]/10);
			display(5,time[select_mode-1]%10);
		}
		else
		{
			display(5,time[select_mode-1]);
		}
		display(6,0);
		display(7,0);
	}
	if(mode==2) //���ѡ����ת���
	{
		display(0,10);
		display(1,select_mode);
		display(2,10);
		if(flag_800ms) //800ms�������˸���
		{
			if(time[select_mode-1]>=10) //�����ת���>=1000����ʾ����λ
			{
				display(4,time[select_mode-1]/10);
				display(5,time[select_mode-1]%10);
			}
			else
			{
				display(5,time[select_mode-1]);
			}
			display(6,0);
			display(7,0);
		}
		else
		{
			display(4,11);
			display(5,11);
			display(6,11);
			display(7,11);
		}
	}
}

void set_pwm()
{
	uchar dat;
	dat=read_pcf8591(3);
	if(dat<=64)
	{
		pwm=25;
	}
	else if(dat<=128)
	{
		pwm=50;
	}	
	else if(dat<=192)
	{
		pwm=75;
	}
	else
	{
		pwm=100;
	}
}

void init_timer1(void)		//100΢��@11.0592MHz
{
	AUXR |= 0x40;		//��ʱ��ʱ��1Tģʽ
	TMOD &= 0x0F;		//���ö�ʱ��ģʽ
	TL1 = 0xAE;		//���ö�ʱ��ֵ
	TH1 = 0xFB;		//���ö�ʱ��ֵ
	TF1 = 0;		//���TF1��־
	TR1 = 1;		//��ʱ��1��ʼ��ʱ
	ET1 = 1;
	EA = 1;
}

void led_run()
{
	switch(stat)
	{
		case 0:
			data_led=0xff;
			break;
		case 1:
			data_led=0xfe;
			break;
		case 2:
			data_led=0xfc;
			break;
		case 3:
			data_led=0xf8;
			break;
		case 4:
			data_led=0xf0;
			break;
		case 5:
			data_led=0xe0;
			break;
		case 6:
			data_led=0xc0;
			break;
		case 7:
			data_led=0x80;
			break;
		case 8:
			data_led=0x00;
			break;
		case 9:
			data_led=0x7f;
			break;
		case 10:
			data_led=0x3f;
			break;
		case 11:
			data_led=0x1f;
			break;
		case 12:
			data_led=0x0f;
			break;
		case 13:
			data_led=0x07;
			break;
		case 14:
			data_led=0x03;
			break;
		case 15:
			data_led=0x01;
			break;
		case 16:
			data_led=0x00;
			break;
		case 17:
			data_led=0x7e;
			break;
		case 18:
			data_led=0xbd;
			break;
		case 19:
			data_led=0xdb;
			break;
		case 20:
			data_led=0xe7;
			break;
		case 21:
			data_led=0xe7;
			break;
		case 22:
			data_led=0xdb;
			break;
		case 23:
			data_led=0xbd;
			break;
		case 24:
			data_led=0x7e;
			break;
	}
}

void ser_timer1() interrupt 3
{
	static uint count=0,pwm_count=0;
	static uchar temp_mode=1;
	uchar temp=P0&0xff;
	uchar channel=(P2&0xe0)>>5;
	count++;
	pwm_count++;
	if(count==time[temp_mode-1]*1000)
	{
		count=0;
		led_run();
		if(stat==24)
		{
			stat=0;
		}
		stat++;
		if(stat==0)
		{
			temp_mode=1;
		}
		else if(stat==9)
		{
			temp_mode=2;
		}
		else if(stat==17)
		{
			temp_mode=3;
		}
		else if(stat==21)
		{
			temp_mode=4;	
		}
	}
	
	if(pwm_count<=pwm)
	{
		P0=0xff;
		select(4);
		P0=data_led;
		select(channel);
		P0=temp;
	}
	else if(pwm_count<100)
	{
		P0=0xff;
		select(4);
		P0=0xff;
		select(channel);
		P0=temp;
	}
	else if(pwm_count==100)
	{
		pwm_count=0;
	}

}

void write_data()
{
	if(flag_write)
	{
		write_at24c02(0x00,time[0]);
		write_at24c02(0x01,time[1]);
		write_at24c02(0x02,time[2]);
		write_at24c02(0x03,time[3]);
	}
}

int main()
{
	init_sys();
	init_timer0();
	init_timer1();
	while(1)
	{
		key_press();
		display_data();
		write_data();
	}
}