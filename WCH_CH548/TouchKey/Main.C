/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/09
* Description        : CH549 16ͨ������������ʾ
*                      CH0~CH15 �ֱ��Ӧ���� P1.0~P1.7  P0.0~P0.7
                       ע�����DEBUG.C
*******************************************************************************/
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
#include ".\TouchKey\TouchKey.H"
#pragma  NOAREGS
UINT16 PowerValue[16];                                                         //���津�������ϵ�δ����ֵ
volatile UINT16 Press_Flag = 0;                                                //���±�־λ
UINT8C CPW_Table[16] = { 30,30,30,30, 30,30,30,30,                             //��������йصĲ������ֱ��Ӧÿ������
                         30,30,30,30, 30,30,30,30,
                       };

/*******************************************************************************
* Function Name  : ABS
* Description    : ����������ֵ�ľ���ֵ
* Input          : a,b
* Output         : None
* Return         : ��ֵ����ֵ
*******************************************************************************/
UINT16 ABS(UINT16 a,UINT16 b)
{
    if(a>b)
    {
        return (a-b);
    }
    else
    {
        return (b-a);
    }
}
/*******************************************************************************
* Function Name  : LED_Port_Init
* Description    : LED���ų�ʼ��,�������
*                  P22~P25
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LED_Port_Init(void)
{
    P2 |= (0xF<<2);                                                          //Ĭ��Ϩ��
    P2_MOD_OC &= ~(0xF<<2);
    P2_DIR_PU |= (0xF<<2);
}
/*******************************************************************************
* Function Name  : LED_Control
* Description    : ��ƿ���
* Input          : LEDx: 0~3 �ֱ��ӦP22~P25�ĸ�LED�ƣ�������
*                  status: 0:��  1����
* Output         : None
* Return         : None
*******************************************************************************/
void LED_Control(UINT8 LEDx,UINT8 status)
{
    if(LEDx>3)
    {
        return;
    }
    if(status)                                                                //����
    {
        P2 &= ~(1<<(2+LEDx));
    }
    else                                                                      //Ϩ��
    {
        P2 |= (1<<(2+LEDx));
    }
}
//������
void main()
{
    UINT8 ch;
    UINT16 value;
    UINT16 err;                                                                //����ģ��仯��ֵ
    CfgFsys( );                                                                //CH549ʱ��ѡ������
    mDelaymS(20);
    mInitSTDIO( );                                                             //����0��ʼ��
    printf("TouchKey demo start ...\n");
    LED_Port_Init();
    TouchKey_Init();
    Press_Flag = 0;                                                            //�ް�������
    /* ��ȡ������ֵ */
    for(ch = 8; ch!=12; ch++)
    {
        PowerValue[ch] = TouchKeySelect( ch,CPW_Table[ch] );
        printf("%d ",PowerValue[ch] );
    }
    printf("\n");
    while(1)
    {
        /* ������� */
        for(ch = 8; ch!=12; ch++)
        {
            value = TouchKeySelect( ch,CPW_Table[ch] );
            err = ABS(PowerValue[ch],value);
            if( err > DOWM_THRESHOLD_VALUE )                                   //��ֵ������ֵ����Ϊ����
            {
                if((Press_Flag & (1<<ch)) == 0)                                //˵���ǵ�һ�ΰ���
                {
                    printf("ch %d pressed,value:%d\n",(UINT16)ch, value);
                    /* ��ƴ��� */
                    LED_Control(ch-8,1);
                }
                Press_Flag |= (1<<ch);
            }
            else if( err < UP_THRESHOLD_VALUE )                                //˵��̧�����δ����
            {
                if(Press_Flag & (1<<ch))                                       //��̧��
                {
                    Press_Flag &= ~(1<<ch);
                    printf("ch %d up,value:%d\n",(UINT16)ch, value);
                    /* ��ƴ��� */
                    LED_Control(ch-8,0);
                }
            }
        }
    }
}