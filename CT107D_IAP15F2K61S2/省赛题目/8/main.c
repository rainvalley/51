#include<stc15f2k60s2.h>
#include"ds1302.h"
#include"onewire.h"
typedef unsigned char uchar;
typedef unsigned int uint;
uchar code dig_code[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x90,0xbf,0xff};
//ʮ���Ʊ�ʾʱ��������
char time[]={58,59,23};
char alarm[]={0,0,0};
//�������壺����������������ģʽ��ʱ��ѡ��λ������ѡ��λ
uchar trg=0,cont=0,mode=0,time_selected=0,alarm_selected=0;
//��־������1s��200ms�����ӱ�־λ��ʱ���ȡλ
uchar flag_1s=0,flag_200ms=0,flag_alarm=0,flag_time=1;
//�¶�
uint temp=0;
void get_temp();
void write_ds1302();
void modify_ds1302();
sbit S4=P3^3;
sbit L1=P0^0;

void delay(uint t)
{
	while(t--);
}

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
	write_ds1302();
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

//ʮ����תBCD
uchar num_to_bcd(uchar num)
{
	uchar a,b;
	a=num/10;
	b=num%10;
	return (16*a+b);
}

//BCDתʮ����
uchar bcd_to_num(uchar bcd)
{
	uchar a,b;
	a=bcd/16;
	b=bcd%16;
	return (10*a+b);
}

void scan_key()
{
	uchar dat=P3^0xff;
	trg=dat&(dat^cont);
	cont=dat;
}

void key_fun()
{
	if(trg==0x01)
	{
		mode=0; //ģʽ0��ʱ��ģʽ
		alarm_selected=0;
		time_selected++;
		if(time_selected>=4) //ʱ��ģʽ�µ�ѡ��0������ʾ��1����ʱ��2�����֡�3������
		{
			time_selected=0;
		}
	}
	if(trg==0x02)
	{
		mode=1; //ģʽ1������ģʽ
		time_selected=0;
		alarm_selected++;
		if(alarm_selected>=4) //����ģʽ�µ�ѡ��0������ʾ��1����ʱ��2�����֡�3������
		{
			alarm_selected=0;
		}
	}
	if(trg==0x04)
	{
		if(mode==0&&time_selected==1) //ʱ��ģʽ�¼���
		{
			time[2]++;
			if(time[2]>=24)
			{
				time[2]=0;
			}
			modify_ds1302();
		}
		if(mode==0&&time_selected==2) //ʱ��ģʽ�¼ӷ�
		{
			time[1]++;
			if(time[1]>=60)
			{
				time[1]=0;
			}
			modify_ds1302();
		}
		if(mode==0&&time_selected==3) //ʱ��ģʽ�¼�ʱ
		{
			time[0]++;
			if(time[0]>=60)
			{
				time[0]=0;
			}
			modify_ds1302();
		}
	}
	if(trg==0x04)
	{
		if(mode==1&&alarm_selected==1) //����ģʽ�¼���
		{
			alarm[2]++;
			if(alarm[2]>=24)
			{
				alarm[2]=0;
			}
		}
		if(mode==1&&alarm_selected==2) //����ģʽ�¼ӷ�
		{
			alarm[1]++;
			if(alarm[1]>=60)
			{
				alarm[1]=0;
			}
		}
		if(mode==1&&alarm_selected==3) //����ģʽ�¼�ʱ
		{
			alarm[0]++;
			if(alarm[0]>=60)
			{
				alarm[0]=0;
			}
		}
	}
	if(trg==0x08)
	{
		if(mode==0&&time_selected==1)
		{
			time[2]--;
			if(time[2]<=-1)
			{
				time[2]=23;
			}
			modify_ds1302();
		}
		if(mode==0&&time_selected==2)
		{
			time[1]--;
			if(time[1]<=-1)
			{
				time[1]=59;
			}
			modify_ds1302();
		}
		if(mode==0&&time_selected==3)
		{
			time[0]--;
			if(time[0]<=-1)
			{
				time[0]=59;
			}
			modify_ds1302();
		}
	}
	if(trg==0x08)
	{
		if(mode==1&&alarm_selected==1)
		{
			alarm[2]--;
			if(alarm[2]<=-1)
			{
				alarm[2]=23;
			}
		}
		if(mode==1&&alarm_selected==2)
		{
			alarm[1]--;
			if(alarm[1]<=-1)
			{
				alarm[1]=59;
			}
		}
		if(mode==1&&alarm_selected==3)
		{
			alarm[0]--;
			if(alarm[0]<=-1)
			{
				alarm[0]=59;
			}
		}
	}
	if(S4==0)
	{
		if(mode==0&&time_selected==0)
		{
			while(S4==0)
			{
				get_temp();
				display(6,temp/10);
				display(7,temp%10);
			}
		}
	}
}

void display_time()
{
	display(0,time[2]/10);
	display(1,time[2]%10);
	display(2,10);
	display(3,time[1]/10);
	display(4,time[1]%10);
	display(5,10);
	display(6,time[0]/10);
	display(7,time[0]%10);
}

void display_alarm()
{
	display(0,alarm[2]/10);
	display(1,alarm[2]%10);
	display(2,10);
	display(3,alarm[1]/10);
	display(4,alarm[1]%10);
	display(5,10);
	display(6,alarm[0]/10);
	display(7,alarm[0]%10);
}

void display_data()
{
	if(mode==0&&time_selected==0)
	{
		display(0,time[2]/10);
		display(1,time[2]%10);
		display(2,10);
		display(3,time[1]/10);
		display(4,time[1]%10);
		display(5,10);
		display(6,time[0]/10);
		display(7,time[0]%10);
	}
	if(mode==0&&time_selected==1)
	{
		if(flag_1s)
		{
			display_time();
		}
		else
		{
			display(0,11);
			display(1,11);
			display(2,10);
			display(3,time[1]/10);
			display(4,time[1]%10);
			display(5,10);
			display(6,time[0]/10);
			display(7,time[0]%10);
		}
	}
	if(mode==0&&time_selected==2)
	{
		if(flag_1s)
		{
			display_time();
		}
		else
		{
			display(0,time[2]/10);
			display(1,time[2]%10);
			display(2,10);
			display(3,11);
			display(4,11);
			display(5,10);
			display(6,time[0]/10);
			display(7,time[0]%10);
		}
	}
	if(mode==0&&time_selected==3)
	{
		if(flag_1s)
		{
			display_time();
		}
		else
		{
			display(0,time[2]/10);
			display(1,time[2]%10);
			display(2,10);
			display(3,time[1]/10);
			display(4,time[1]%10);
			display(5,10);
			display(6,11);
			display(7,11);
		}
	}
	
	if(mode==1&&alarm_selected==0)
	{
		display_alarm();
	}
	if(mode==1&&alarm_selected==1)
	{
		if(flag_1s)
		{
			display_alarm();
		}
		else
		{
			display(0,11);
			display(1,11);
			display(2,10);
			display(3,alarm[1]/10);
			display(4,alarm[1]%10);
			display(5,10);
			display(6,alarm[0]/10);
			display(7,alarm[0]%10);
		}
	}
	if(mode==1&&alarm_selected==2)
	{
		if(flag_1s)
		{
			display_alarm();
		}
		else
		{
			display(0,alarm[2]/10);
			display(1,alarm[2]%10);
			display(2,10);
			display(3,11);
			display(4,11);
			display(5,10);
			display(6,alarm[0]/10);
			display(7,alarm[0]%10);
		}
	}
	if(mode==1&&alarm_selected==3)
	{
		if(flag_1s)
		{
			display_alarm();
		}
		else
		{
			display(0,alarm[2]/10);
			display(1,alarm[2]%10);
			display(2,10);
			display(3,alarm[1]/10);
			display(4,alarm[1]%10);
			display(5,10);
			display(6,11);
			display(7,11);
		}
	}
}

void init_timer0(void)		//1����@11.0592MHz
{
	AUXR &= 0x7F;		//��ʱ��ʱ��12Tģʽ
	TMOD &= 0xF0;		//���ö�ʱ��ģʽ
	TL0 = 0x66;		//���ö�ʱ��ֵ
	TH0 = 0xFC;		//���ö�ʱ��ֵ
	TF0 = 0;		//���TF0��־
	TR0 = 1;		//��ʱ��0��ʼ��ʱ
	ET0 = 1;
	EA = 1;
}

void ser_timer0() interrupt 1
{
	static uint count_1s=0,count_50ms=0,count_200ms=0;
	count_1s++;
	count_50ms++;
	count_200ms++;
	if(count_200ms>=200)
	{
		count_200ms=0;
		flag_200ms=~flag_200ms;
	}
	if(count_1s>=1000)
	{
		count_1s=0;
		flag_1s=~flag_1s;
	}
	if(count_50ms>=50)
	{
		count_50ms=0;
		scan_key();
		key_fun();
	}
	
}

void get_temp()
{
	uchar LSB,MSB;
	EA=0;
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0x44);
	init_ds18b20();
	Write_DS18B20(0xcc);
	Write_DS18B20(0xbe);
	LSB=Read_DS18B20();
	MSB=Read_DS18B20();
	init_ds18b20();
	temp=(MSB<<8)|LSB;
	temp=temp>>4;
	EA=1;
}

void write_ds1302()
{
	uchar address=0x80;
	uchar i;
	Write_Ds1302(0x8e,0x00);
	for(i=0;i<3;i++)
	{
		Write_Ds1302(address,num_to_bcd(time[i]));
		address=address+2;
	}
	Write_Ds1302(0x8e,0x80);
}

void read_ds1302()
{
	uchar address=0x81;
	uchar i;
	for(i=0;i<3;i++)
	{
		time[i]=bcd_to_num(Read_Ds1302(address));
		address=address+2;
	}
}

void modify_ds1302() //�޸�ʱ����ֹͣ��ȡ
{
	flag_time=0;
	write_ds1302();
	flag_time=1;
}

void check_alarm()
{
	uchar i=0,count=0;
	for(i=0;i<3;i++)
	{
		if(time[i]==alarm[i])
		{
			count++;
		}
	}
	if(count==3)
	{
		flag_alarm=1;
	}
}

void led_alarm()
{
	static uchar count=0,flag_backup=1;
	if(flag_alarm)
	{
		if(flag_200ms)
		{
			select(4);
			L1=1; //���ɼ�ȡ������ΪP0��������ϱ�����
			select(0);
		}
		else
		{
			select(4);
			L1=0;
			select(0);
		}
		if(flag_200ms!=flag_backup) //���ڱ仯ʱ�ۼӼ�����������ѭ������
		{
			count++;
			flag_backup=flag_200ms;
		}
		if(count>=25||P3!=0xff) //���25����˸�����ж�������������
		{
			count=0;
			flag_alarm=0;
			select(4);
			P0=0xff;
			select(0);
		}
	}
}

int main()
{
	init_sys();
	init_timer0();
	while(1)
	{
		check_alarm();
		led_alarm();
		if(flag_time)
		{
			read_ds1302();
		}
		display_data();
	}
}