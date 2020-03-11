#include "infrared.h"
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"

/*********************************************
** �������� ������ͨ��
** ��    ��	�� 
** ��    ��	�� ��
** ��    ��	�� ��
** ��    ע �� �й�ʽ�����뼴��
*********************************************/
UINT8 INF_CNT_f=0;//��־λ1--�������0--���ڴ���
UINT8 IF_DC_KEY=0;//����ֵ1--17

void Int1Init()
{
	   //����INT1
	IT1=1;//�����س�����ʽ���½��أ�
	EX1=1;//��INT1���ж�����	
	EA=1;//�����ж�

}

void Infrared_Communication() interrupt INT_NO_INT1
{
   
  UINT16 rx_tuff;
  UINT16 err;//��ֹ������
  UINT8 i=0,j=0,k=0;
  UINT8 addr[4]={0};
	
  for(i=0;i<200;i++)//��ʼ��ǰ���9ms�͵�ƽ
  {
    mDelayuS(15);
    if(Pin_Decode==1)//�����ָߵ�ƽ������
    {
      return;//��������
    }
  }
  err=5000;
  while(  (Pin_Decode==0) && (err>1) )//�ȴ���ʼ���벿�ֵ�4.5ms�ߵ�ƽ�ĵ���
  {
    err--;mDelayuS(10);
  }
  for(i=0;i<100;i++)
  {
    mDelayuS(10);
    if(Pin_Decode==0)//�����ֵ͵�ƽ����ʼ��������������
    {
      return;//��������
    }
  }
 
  //׼���ý���32λң������
  for(i=0;i<4;i++)
  {
    for(j=0;j<8;j++)
    {
      err=5000;
      while( (Pin_Decode==1) && (err>1) )//�ȴ���ַ��ĵ�һλ��ʼ����ĵ���  0.56ms  0
      {
        err--;mDelayuS(10);
      }
      err=2000;
      while( (Pin_Decode==0) && (err>1) )//�ȴ���ַ��ĵ�һλ�ߵ�ƽ�ĵ���
      {
        err--;mDelayuS(10);
      }
      while( Pin_Decode  )//����ߵ�ƽ��ʱ�䳤��
      {
         mDelayuS(100);
        if((k++)>=30)
        {
          return;
        }
      }	
      addr[i]=addr[i]>>1;
      if(k>8)
        addr[i]=addr[i]|0x80;
      k=0;
    }
  }
  rx_tuff=~(addr[2]);
  switch(rx_tuff)
  {
  case 65466:IF_DC_KEY=1;break;
  case 65465:IF_DC_KEY=2;break;
  case 65464:IF_DC_KEY=3;break;
  case 65467:IF_DC_KEY=4;break;
  case 65471:IF_DC_KEY=5;break;
  case 65468:IF_DC_KEY=6;break;
  case 65528:IF_DC_KEY=7;break;
  case 65514:IF_DC_KEY=8;break;
  case 65526:IF_DC_KEY=9;break;
  case 65510:IF_DC_KEY=0;break;
  case 65513:IF_DC_KEY=11;break;//*
  case 65522:IF_DC_KEY=12;break;//#
  case 65511:IF_DC_KEY=13;break;//��
  case 65507:IF_DC_KEY=15;break;//OK
  case 65527:IF_DC_KEY=14;break;//��
  case 65445:IF_DC_KEY=16;;break;//��
  case 65453:IF_DC_KEY=17;break;//��
  default :IF_DC_KEY=100;break;// 11 ������ֵ
  }
  
  INF_CNT_f=1;
		
	
}