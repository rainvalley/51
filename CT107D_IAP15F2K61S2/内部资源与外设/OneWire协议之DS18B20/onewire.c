/*
  ����˵��: ��������������
  �������: Keil uVision 4.10 
  Ӳ������: CT107��Ƭ���ۺ�ʵѵƽ̨(�ⲿ����12MHz) STC89C52RC��Ƭ��
  ��    ��: 2011-8-9
  �޸�ʱ�䣺2020-1-19
  �޸Ļ�����IAP15F2K61S2
*/
#include "reg52.h"
#include "onewire.h"

sbit DQ = P1^4;  //�����߽ӿ�

//��������ʱ����
void Delay_OneWire(unsigned int t)  //STC89C52RC
{
	t=t*12;//����12�ӳ�
	while(t--);
}

//ͨ����������DS18B20дһ���ֽ�
void Write_DS18B20(unsigned char dat)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		DQ = 0; //��������10-15us
		DQ = dat&0x01; //������д������
		Delay_OneWire(5); //ά������20-45us 
		DQ = 1; //�ͷ�����
		dat >>= 1; //׼��������һ������λ
	}
	Delay_OneWire(5);
}

//��DS18B20��ȡһ���ֽ�
unsigned char Read_DS18B20(void)
{
	unsigned char i;
	unsigned char dat;
  
	for(i=0;i<8;i++)
	{
		DQ = 0; //��������10-15us
		dat >>= 1;
		DQ = 1; //�ͷ����ߺ�
		if(DQ)
		{
			dat |= 0x80;
		}	    
		Delay_OneWire(5); //��ȡ���ߵ�ƽ���ӳ�45us
	}
	return dat;
}

//DS18B20�豸��ʼ��
bit init_ds18b20(void)
{
  	bit initflag = 0;
  	
  	DQ = 1;
  	Delay_OneWire(12);
  	DQ = 0;
  	Delay_OneWire(80);
  	DQ = 1; //��������480us���ϣ�Ȼ���ͷ�
  	Delay_OneWire(10); 
    initflag = DQ; //�ȴ�15-60us�󣬽�DQд��initflag����Ϊ����DS18B20�ɹ�Ӧ��
  	Delay_OneWire(5);
  
  	return initflag;
}






