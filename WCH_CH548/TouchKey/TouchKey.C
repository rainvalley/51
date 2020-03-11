/********************************** (C) COPYRIGHT *******************************
* File Name          : TouchKey.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/09
* Description        : CH549 TouchKey触摸按键
*                      CH0~CH15 分别对应引脚 P1.0~P1.7  P0.0~P0.7
*******************************************************************************/
#include ".\TouchKey\TouchKey.H"
#pragma  NOAREGS
#if EN_ADC_INT
UINT8  GetValueFlag = 0;                             //获取到有效值标志
UINT16 IntCurValue = 0;                              //中断采集到的当前值
#endif
/*******************************************************************************
* Function Name  : TouchKey_Init
* Description    : 触摸按键初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TouchKey_Init( void )
{
                                                     //Touch采样通道设置为高阻输入
    P0_MOD_OC &= 0xF0;                               //P00 P01 P02 P03高阻输入
    P0_DIR_PU &= 0xF0;
    ADC_CFG |= (bADC_EN|bADC_AIN_EN);                //开启ADC模块电源,选择外部通道
    ADC_CFG = ADC_CFG & ~(bADC_CLK0 | bADC_CLK1);    //选择ADC参考时钟
    ADC_CHAN = (3<<4);                               //默认选择外部通道0
    ADC_CTRL = bADC_IF;                              //清除ADC转换完成标志，写1清零
#if EN_ADC_INT
    SCON2 &= bU2IE;                                  //和UART2中断地址共用，故中断需2选1
    IE_ADC = 1;                                      //开启ADC中断使能
    EA = 1;                                          //开启总中断使能
#endif
}
/*******************************************************************************
* Function Name  : TouchKey_Init
* Description    : 触摸按键初始化
* Input          : 通道号选择ch:0~15,分别对应P10~P17、P00~P07
                   充电脉冲宽度 cpw:0~127
                   cpw由 外部触摸按键电容、VDD电压、主频三者决定。
                   计算公式：count=(Ckey+Cint)*0.7VDD/ITKEY/(2/Fsys)
                   TKEY_CTRL=count > 127 ? 127 : count （其中Cint为15pF,ITKEY为50u）
                   简化公式：cpw = （Ckey+15）*0.35*VDD*Fsys/50
                   cpw = cpw>127?127:cpw
* Output         : None
* Return         : 返回触摸检测电压
*******************************************************************************/
UINT16 TouchKeySelect( UINT8 ch,UINT8 cpw )
{
    ADC_CHAN = ADC_CHAN & (~MASK_ADC_CHAN) | ch;     //外部通道选择
    //电容较大时可以先设置IO低，然后恢复浮空输入实现手工放电，≤0.2us
#if EN_ADC_INT
    GetValueFlag = 0;                                //标志位清0
#endif
    TKEY_CTRL = cpw;                                 //充电脉冲宽度配置，仅低7位有效（同时清除bADC_IF，启动一次TouchKey）
#if EN_ADC_INT
    while(GetValueFlag == 0);                        //等待采用完成
#else
    while(ADC_CTRL&bTKEY_ACT);
#endif
    return (ADC_DAT&0xFFF);
}

#if EN_ADC_INT
/*******************************************************************************
* Function Name  : ADC_ISR( )
* Description    : ADC 中断服务函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ADC_ISR(void)  interrupt INT_NO_ADC
{
    if(ADC_CTRL&bADC_IF)
    {
        ADC_CTRL = bADC_IF;                          //清除ADC转换完成标志
        IntCurValue = (ADC_DAT&0xFFF);               //采样值保存到变量IntCurValue
        GetValueFlag = 1;                            //采样完成标志
    }
}
#endif
