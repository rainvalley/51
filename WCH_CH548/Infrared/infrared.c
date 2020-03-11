#include "infrared.h"
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"

/*********************************************
** 函数功能 ：红外通信
** 输    入	： 
** 输    出	： 无
** 返    回	： 无
** 备    注 ： 有公式，带入即可
*********************************************/
UINT8 INF_CNT_f=0;//标志位1--处理完成0--正在处理
UINT8 IF_DC_KEY=0;//按键值1--17

void Int1Init()
{
	   //设置INT1
	IT1=1;//跳变沿出发方式（下降沿）
	EX1=1;//打开INT1的中断允许。	
	EA=1;//打开总中断

}

void Infrared_Communication() interrupt INT_NO_INT1
{
   
  UINT16 rx_tuff;
  UINT16 err;//防止程序卡死
  UINT8 i=0,j=0,k=0;
  UINT8 addr[4]={0};
	
  for(i=0;i<200;i++)//起始码前面的9ms低电平
  {
    mDelayuS(15);
    if(Pin_Decode==1)//若出现高电平则有误
    {
      return;//跳出函数
    }
  }
  err=5000;
  while(  (Pin_Decode==0) && (err>1) )//等待起始码后半部分的4.5ms高电平的到来
  {
    err--;mDelayuS(10);
  }
  for(i=0;i<100;i++)
  {
    mDelayuS(10);
    if(Pin_Decode==0)//若出现低电平则起始码有误，跳出解码
    {
      return;//跳出函数
    }
  }
 
  //准备好接收32位遥控数据
  for(i=0;i<4;i++)
  {
    for(j=0;j<8;j++)
    {
      err=5000;
      while( (Pin_Decode==1) && (err>1) )//等待地址码的第一位开始脉冲的到来  0.56ms  0
      {
        err--;mDelayuS(10);
      }
      err=2000;
      while( (Pin_Decode==0) && (err>1) )//等待地址码的第一位高电平的到来
      {
        err--;mDelayuS(10);
      }
      while( Pin_Decode  )//计算高电平的时间长度
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
  case 65511:IF_DC_KEY=13;break;//上
  case 65507:IF_DC_KEY=15;break;//OK
  case 65527:IF_DC_KEY=14;break;//左
  case 65445:IF_DC_KEY=16;;break;//右
  case 65453:IF_DC_KEY=17;break;//下
  default :IF_DC_KEY=100;break;// 11 错误反馈值
  }
  
  INF_CNT_f=1;
		
	
}