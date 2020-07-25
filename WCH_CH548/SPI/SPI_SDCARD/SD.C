/********************************** (C) COPYRIGHT *******************************
* File Name          : SD.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/28
* Description        : CH549 SD��������д�Ȼ�������
*******************************************************************************/
/********************************* ���Ŷ��� ************************************
*    P14  <===========>  SCS
*    P15  <===========>  DI/MOSI
*    P16  <===========>  DO/MISO
*    P17  <===========>  SCK
*    P13  <===========>  C/D
*******************************************************************************/
#include ".\SPI\SPI_SDCARD\SD.H"
SD_INFO xdata sd_info;                                                         //SD����Ϣ
UINT8X param[4];                                                               //SD�������������ֽڲ���
UINT8X resp[16];                                                               //SD����Ӧ
UINT32X tr;                                                                    //32λȫ�ֱ���
/*******************************************************************************
* Function Name  : SPI_TransByte(UINT8 dat)
* Description    : SPI �շ�һ���ֽ�
* Input          : UINT8 dat д����
* Output         : None
* Return         : ��������
*******************************************************************************/
UINT8 SPI_TransByte(UINT8 dat)
{
    SPI0_DATA = dat;
    while(S0_FREE == 0)
    {
        ;
    }
    return SPI0_DATA;
}
/*******************************************************************************
* Function Name  : SD_SendCmd
* Description    : ������������Ӧ
* Input          : cmd ������
*                  param ָ�����ֽڲ���
* Output         : resp ָ�����������
* Return         : ����״̬
*******************************************************************************/
UINT8 SD_SendCmd(UINT8 cmd,UINT8X *param,UINT8X *resp )
{
    UINT8X s;
    SPI_TransByte(0x40 | (cmd&0x3f));
    SPI_TransByte(param[0]);
    SPI_TransByte(param[1]);
    SPI_TransByte(param[2]);
    SPI_TransByte(param[3]);
    if(cmd == CMD0)
    {
        SPI_TransByte(0x95);
    }
    else if(cmd == CMD8)
    {
        SPI_TransByte(0x87);
    }
    else
    {
        SPI_TransByte(0xFF);
    }
    if(cmd == CMD12)
    {
        SPI_TransByte(0xFF);    //������cmd12֮�������һ���ֽ���Ҫ����
    }
    tr = 0;
    do
    {
        s = SPI_TransByte(0xff);
        tr++;
    }
    while(( ( s & 0x80 ) != 0x00 ) && ( tr < SD_CMD_TIMEOUT ));
    if(tr>=SD_CMD_TIMEOUT)
    {
        return (SD_ERR_CMD_TIMEOUT);                                           //��Ӧ��ʱ
    }
    *resp = s;
    if(s&0x04)
    {
        return SD_CMD_UNSURPORT;                                               //��֧�ֵ�����
    }
    else if(s&0x40)
    {
        return SD_ERR_CMD_PARAM;    //��������
    }
    else if(s&0x08)
    {
        return SD_ERR_CMD_CRC;    //CRC����
    }
    return SD_ERR_CMD_OK;
}
/*******************************************************************************
* Function Name  : POWER_ON
* Description    : SD���ϵ�,����80��ʱ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void POWER_ON(void)
{
    SCS = 1;
    for(tr=0; tr!=10; tr++)
    {
        SPI_TransByte(0xFF);
    }
}
/*******************************************************************************
* Function Name  : SD_Reset
* Description    : SD����λ���� CMD0
* Input          : None
* Output         : resp ָ�����������
* Return         : ����ִ��״̬
*******************************************************************************/
UINT8 SD_Reset(UINT8X *resp)
{
    UINT8X s;
    memset(param,0,4);
    SCS = 0;
    s = SD_SendCmd(CMD0,param,resp);
    SCS = 1;
    return s;
}
/*******************************************************************************
* Function Name  : SD_CHECK_VOLTAGE
* Description    : ���ڳ�ʼ������������ǿ�Ƽ���CMD8��������֤��������
* Input          : None
* Output         : resp ָ�����������
* Return         : ����ִ��״̬
*******************************************************************************/
UINT8 SD_CHECK_VOLTAGE(UINT8X *resp)
{
    UINT8X s;
    UINT8X res_r0;
    param[0] = 0;
    param[1] = 0;
    param[2] = 1;
    param[3] = 0xAA;
    SCS = 0;
    s = SD_SendCmd(CMD8,param,&res_r0);
    if(s==SD_ERR_CMD_OK)
    {
        for(tr=0; tr<4; tr++)
        {
            resp[tr]= SPI_TransByte(0xff);
        }
    }
    SCS = 1;
    return s;
}
/*******************************************************************************
* Function Name  : SD_GET_OCR
* Description    : ��ȡOCR����ʼ��SD������ɺ󣬴�Idle����Ready״̬
* Input          : None
* Output         : resp ָ�����������
* Return         : ����ִ��״̬
*******************************************************************************/
UINT8 SD_GET_OCR(UINT8X *resp)
{
    UINT8X  s;
    UINT8X res_r0;
    memset(param,0,4);
    for(tr=0; tr!=SD_CMD_TIMEOUT; tr++)
    {
        memset(param,0,4);
        SCS = 0;
        s = SD_SendCmd(CMD55,param,&res_r0);
        SCS = 1;
        if(s==SD_ERR_CMD_OK)
        {
            if(sd_info.type==0x02)                                              //HCS(BIT30)��Ҫ��1
            {
                param[0]=0x40;
            }
            else                                                                //HCSӦ������Ϊ0
            {
                param[0]=0x00;
            }
            param[1]=0xff;
            param[2]=0x80;
            param[3]=0x00;
            SCS = 0;
            s = SD_SendCmd(ACMD41,param,&res_r0);
            SCS = 1;
            if(s==SD_ERR_CMD_OK)
            {
                if((res_r0 & 0x01)==0)                                          //�˳�Idle
                {
                    break;
                }
            }
            else if(s==SD_CMD_UNSURPORT)
            {
                if(sd_info.type==0x01)
                {
                    sd_info.type=0x00;                                         //������MMC��
                    for(tr=0; tr!=SD_CMD_TIMEOUT; tr++)
                    {
                        memset(param,0,4);
                        SCS = 0;
                        s = SD_SendCmd(CMD1,param,&res_r0);                    //mmc���ĳ�ʼ����cmd1��ʼ
                        SCS = 1;
                        if(s==SD_ERR_CMD_OK)
                        {
                            if((res_r0 & 0x01)==0)                             //�˳�Idle
                            {
                                break;
                            }
                        }
                    }
                }
                break;
            }
            else
            {
                return s;
            }
        }
        else
        {
            return s;
        }
    }
    memset(param,0,4);
    SCS = 0;
    s = SD_SendCmd(CMD58,param,&res_r0);                                       //ִ��CMD58��ȡOCR
    if(s == SD_ERR_CMD_OK)
    {
        for(tr=0; tr<4; tr++)
        {
            resp[tr]= SPI_TransByte(0xff);
        }
        if(resp[0]&0x80)
        {
            if(resp[0]&0x40)                                                   //����CCSλ����ֻҪtype����3����CCS����0
            {
                sd_info.type = 3;    //��������
            }
        }
    }
    SCS = 1;
    return s;
}
/*******************************************************************************
* Function Name  : SD_GET_CSD
* Description    : ��ȡCSD�Ĵ���(�ṩ���ڷ��ʿ����ݵ���Ϣ)
* Input          : None
* Output         : resp ָ�����������
* Return         : ����ִ��״̬
*******************************************************************************/
UINT8 SD_GET_CSD(UINT8X *resp)
{
    UINT8X  s;
    UINT8X res_r0;
    memset(param,0,4);
    SCS = 0;
    s = SD_SendCmd(CMD9,param,&res_r0);
    if(s!=SD_ERR_CMD_OK)
    {
        SCS = 1;
        return s;
    }
    tr= 0;
    do
    {
        s = SPI_TransByte(0xff);
        tr++;
    }
    while(( s != 0xFE ) && ( tr < SD_CMD_TIMEOUT ));
    if(tr>=SD_CMD_TIMEOUT)
    {
        SCS = 1;
        return (SD_ERR_CMD_TIMEOUT);                                          //��Ӧ��ʱ
    }
    for(tr=0; tr<16; tr++)
    {
        resp[tr]= SPI_TransByte(0xff);
    }
    SPI_TransByte(0xff);                                                      //�����ֽڵ�CRC
    SPI_TransByte(0xff);
    SCS = 1;
    return SD_ERR_CMD_OK;
}
/*******************************************************************************
* Function Name  : SET_BLOCK_LEN
* Description    : ���ÿ鳤��512�ֽ�(ע�����������ǿ��Ϊ512�ֽ�)
* Input          : len ���ÿ鳤��
* Output         : None
* Return         : ����ִ��״̬
*******************************************************************************/
UINT8 SET_BLOCK_LEN(UINT32 len)
{
    UINT8X  s;
    UINT8X res_r0;
    param[0]=(UINT8)(len>>24&0xff);
    param[1]=(UINT8)(len>>16&0xff);
    param[2]=(UINT8)(len>>8&0xff);
    param[3]=(UINT8)(len&0xff);
    SCS = 0;
    s = SD_SendCmd(CMD16,param,&res_r0);
    if(res_r0!=0)
    {
        s = res_r0;    //����res_r0
    }
    SCS = 1;
    return s;
}
/*******************************************************************************
* Function Name  : SD_HIGH_SPEED
* Description    : �л�������ͨѶģʽ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SD_HIGH_SPEED(void)
{
    SPI_CK_SET(6);
}
/*******************************************************************************
* Function Name  : SD_Check_Insert
* Description    : ���SD���Ƿ����,�����򷵻�TRUE; ���򷵻�FALSE
* Input          : None
* Output         : None
* Return         : FALSE or TRUE
*******************************************************************************/
UINT8 SD_Check_Insert( void )
{
    if( SD_INSERT_PIN_IN( ) )                                                 //SD��δ����
    {
        sd_info.insert = 0x00;
        return( FALSE );
    }
    else
    {
        mDelayuS( 1 );
        if( !SD_INSERT_PIN_IN( ) )                                            //SD������
        {
            sd_info.insert = 0x01;
            return( TRUE );
        }
        else                                                                  //SD��δ����
        {
            return( FALSE );
        }
    }
}
/*******************************************************************************
* Function Name  : SD_Init
* Description    : SD����ʼ��
* Input          : None
* Output         : None
* Return         : SD_ERR_CMD_OK �ɹ��� ����
*******************************************************************************/
UINT8 SD_Init(void)
{
    UINT8X s;
    UINT16X tmp;
    if( SD_Check_Insert() == FALSE )
    {
        return SD_ERR_INSERT;
    }
    memset(&sd_info,0,sizeof(SD_INFO));
    SPIMasterModeSet(3);                                                      //SPIģʽ3���˿ڳ�ʼ��
    SPI_CK_SET(64);                                                           //SPI��Ƶ,SD����ʼ��ʱ�Ӳ�����400Khz
    POWER_ON();                                                               //�ϵ�
    /* ����CMD0 */
#if DE_PRINTF
    printf("CMD0:\n");
#endif
    SD_Reset(&resp[0]);                                                       //����λ,����R0
    if(resp[0]!=0x01)
    {
        return SD_ERR_INIT;
    }
    /* ����CMD8����֤ SD ���ӿڲ���������*/
#if DE_PRINTF
    printf("CMD8:\n");
#endif
    s = SD_CHECK_VOLTAGE(&resp[0]);                                           //���س���4
    if(s==SD_CMD_UNSURPORT)
    {
        sd_info.type = 0x01;    //v1����mmc v3��
    }
    else if(s==SD_ERR_CMD_OK)
    {
#if DE_PRINTF
        printf("%02x %02x %02x %02x\n",(UINT16)resp[0],(UINT16)resp[1],(UINT16)resp[2],(UINT16)resp[3]);
#endif
        if(resp[2]==0x01 && resp[3]==0xaa)                                    //��ѹ������2.7-3.6
        {
            sd_info.type = 0x02;
        }
        else
        {
            sd_info.type = 0x04;
        }
    }
    else
    {
        return s;
    }
    /* ��ȡOCR�Ĵ���������ACMD41֮�����Ready״̬�����϶���Idle������ʼ�� */
#if DE_PRINTF
    printf("GET OCR:\n");
#endif
    SD_GET_OCR(&resp[0]);                                                    //����4�ֽ�OCR
#if DE_PRINTF
    printf("Card Type:0x%02x\n",(UINT16)sd_info.type);
#endif
    /* ��ȡCSD����ѡ��CMD9 */
#if DE_PRINTF
    printf("GET CSD:\n");
#endif
    s = SD_GET_CSD(&resp[0]);                                                //����16�ֽ�CSD
    if(s!=SD_ERR_CMD_OK)
    {
        return s;
    }
#if DE_PRINTF
    for(s=0; s!=16; s++)
    {
        printf("%02x ",(UINT16)resp[s]);
    }
    printf("\n");
#endif
    /* ����SD�������Ĵ���,��һ�桢�ڶ��治һ�� */
    if( ( resp[ 0 ] & 0x80 ) == 0x80 )                                       //CSD���ݴ���,������0
    {
        return( SD_ERR_VOL_NOTSUSP );
    }
    else
    {
        /* READ_BL_LEN: λ83--80: 1001------512byte */
        sd_info.block_len = 1 << ( resp[ 5 ] & 0x0F );
        if( sd_info.block_len < 512 )
        {
            return ( SD_BLOCK_ERROR );
        }
        if( ( resp[ 0 ] & 0x40 ) == 0x40  )                                  //CSD�ṹ�汾:�ڶ���
        {
            /* ���ڵڶ��������������:
               λ83--80:------READ_BL_LEN:
               λ69--48:------C_SIZE
               ���� = READ_BL_LEN * ( C_SIZE + 1 ) */
#if DE_PRINTF
            printf("csd struct v2\n");
#endif
            sd_info.block_num = ( ( UINT32 )( resp[ 7 ] & 0x3F ) ) << 16 ;
            sd_info.block_num += ( UINT32 )resp[ 8 ] << 8;
            sd_info.block_num += ( UINT32 )resp[ 9 ];
            sd_info.block_num += 1;
            sd_info.block_num = sd_info.block_num << 10;
            /* ���������� */
            sd_info.capability = ( ( UINT32 ) sd_info.block_len ) * ( ( UINT32 )( sd_info.block_num / 1024 ) );
        }
        else                                                                 //CSD�ṹ�汾:��һ��
        {
            /* ���ڵ�һ�������������:
               λ83--80:------READ_BL_LEN:
               λ73--62:------C_SIZE
               λ49--47:------C_SIZE_MULT
               BLOCKNR = ( C_SIZE + 1 ) * MULT
               MULT = ( C_SIZE_MULT < 8 ) * 2 ^ ( C_SIZE_MULT + 2 )
               BLOCK_LEN = ( READ_BL_LEN < 12 ) * 2 ^ ( READ_BL_LEN )
               �洢������ = BLOCKNR * BLOCK_LEN */
            /* ����C_SIZE_MULT */
#if DE_PRINTF
            printf("csd struct v1\n");
#endif
            tmp  = ( resp[ 9 ] & 0x03 ) << 1;
            tmp += ( ( resp[ 10 ]& 0x80 ) >> 7 ) + 2;
            /* ����C_SIZE */
            sd_info.block_num  = ( resp[ 6 ] & 0x03 ) << 10;
            sd_info.block_num += ( resp[ 7 ] << 2 );
            sd_info.block_num += ( ( resp[ 8 ] & 0xc0 ) >> 6 );
            /* ��ÿ��п������ */
            sd_info.block_num = ( sd_info.block_num + 1 ) * ( 1 << tmp );
            /* ���������� */
            sd_info.capability = ( ( UINT32 ) sd_info.block_len ) * ( ( UINT32 ) ( sd_info.block_num / 1024 ) );
        }
#if DE_PRINTF
        printf("sd_info.block_len = %d\n",(UINT16)sd_info.block_len);
        printf("sd_info.block_num = %ld\n",( UINT32 )sd_info.block_num );
        printf("capability =  %f G\n",( float )( ( ( float )( sd_info.capability / 1024 ) / 1024 )  )  );
#endif
    }
    /* ���ÿ鳤��CMD16 */
    s = SET_BLOCK_LEN(512);
    if(s!=SD_ERR_CMD_OK)
    {
        return s;
    }
    SD_HIGH_SPEED();                                                         //�������ģʽ
    sd_info.init_success = 1;                                                //��ʼ�����
    return SD_ERR_CMD_OK;
}
/*******************************************************************************
* Function Name  : SD_READ_SECTOR
* Description    : ��һ������,���ڡ��������������������ţ����ڡ�С������������������ַ��
*                  Ϊ��ͳһ���ú�������Ĳ���һ��Ϊ�����ţ������ڷ����Ǵ󿨻���С����
*                  �����Ƿ�֧��CMD8���жϡ�
* Input          : sector_num ������
* Output         : datbuf ָ�����������
* Return         : SD_ERR_CMD_OK �ɹ��� ����
*******************************************************************************/
UINT8 SD_READ_SECTOR(UINT8X *datbuf,UINT32 sector_num)
{
    UINT8X  s;
    UINT8X res_r0;
    if(sd_info.type!=3)
    {
        sector_num = sector_num<<9;    //����512��
    }
    param[0]=(UINT8)(sector_num>>24&0xff);
    param[1]=(UINT8)(sector_num>>16&0xff);
    param[2]=(UINT8)(sector_num>>8&0xff);
    param[3]=(UINT8)(sector_num&0xff);
    SCS = 0;
    s = SD_SendCmd(CMD17,param,&res_r0);
    if(s!=SD_ERR_CMD_OK)
    {
        SCS = 1;
        return s;
    }
    tr= 0;
    do
    {
        s = SPI_TransByte(0xff);
        tr++;
    }
    while(( s != 0xFE ) && ( tr < SD_CMD_TIMEOUT ));
    if(tr>=SD_CMD_TIMEOUT)
    {
        SCS = 1;
        return (SD_ERR_CMD_TIMEOUT);                                         //��Ӧ��ʱ
    }
    for(tr=0; tr<512; tr++)
    {
        *datbuf++= SPI_TransByte(0xff);
    }
    SPI_TransByte(0xff);                                                     //�����ֽڵ�CRC
    SPI_TransByte(0xff);
    SCS = 1;
    return SD_ERR_CMD_OK;
}
/*******************************************************************************
* Function Name  : SD_READ_MULT_SECTOR
* Description    : ��������������ڡ��������������������ţ����ڡ�С������������������ַ��
*                  Ϊ��ͳһ���ú�������Ĳ���һ��Ϊ�����ţ������ڷ����Ǵ󿨻���С����
* Input          : sector_num ������
*                  blocks ������
* Output         : datbuf ָ�����������
* Return         : SD_ERR_CMD_OK �ɹ��� ����
*******************************************************************************/
UINT8 SD_READ_MULT_SECTOR(UINT8X *datbuf,UINT32 sector_num,UINT16 blocks )
{
    UINT8X  s;
    UINT8X res_r0;
    if(sd_info.type!=3)
    {
        sector_num = sector_num<<9;    //����512��
    }
    param[0]=(UINT8)(sector_num>>24&0xff);
    param[1]=(UINT8)(sector_num>>16&0xff);
    param[2]=(UINT8)(sector_num>>8&0xff);
    param[3]=(UINT8)(sector_num&0xff);
    SCS = 0;
    s = SD_SendCmd(CMD18,param,&res_r0);
    if(s!=SD_ERR_CMD_OK)
    {
        SCS = 1;
        return s;
    }
    while(blocks--)
    {
        tr= 0;
        do
        {
            s = SPI_TransByte(0xff);
            tr++;
        }
        while(( s != 0xFE ) && ( tr < SD_CMD_TIMEOUT ));
        if(tr>=SD_CMD_TIMEOUT)
        {
            SCS = 1;
            return (SD_ERR_CMD_TIMEOUT);                                     //��Ӧ��ʱ
        }
        for(tr=0; tr<512; tr++)
        {
            *datbuf++= SPI_TransByte(0xff);
        }
        SPI_TransByte(0xff);                                                 //�����ֽڵ�CRC
        SPI_TransByte(0xff);
    }
    /* ����CMD12��ֹ���� */
    memset(param,0,4);
    s = SD_SendCmd(CMD12,param,&res_r0);
    if(s!=SD_ERR_CMD_OK)
    {
        SCS =1;
        return s;
    }
    tr= 0;                                                                   //busy��⣬DO�ź���æ������͵�ƽ
    do
    {
        s = SPI_TransByte(0xff);
        tr++;
    }
    while(( s !=0xFF ) && ( tr < SD_CMD_TIMEOUT ));
    if(tr>=SD_CMD_TIMEOUT)
    {
        SCS = 1;
        return (SD_ERR_CMD_TIMEOUT);                                        //��Ӧ��ʱ
    }
    SCS = 1;
    return SD_ERR_CMD_OK;
}
/*******************************************************************************
* Function Name  : SD_WRITE_SECTOR
* Description    : дһ�����������ڡ��������������������ţ����ڡ�С������������������ַ��
*                  Ϊ��ͳһ���ú�������Ĳ���һ��Ϊ�����ţ������ڷ����Ǵ󿨻���С����
* Input          : sector_num ������
*                  datbuf ָ��д�뻺����
* Output         : None
* Return         : SD_ERR_CMD_OK �ɹ��� ����
*******************************************************************************/
UINT8 SD_WRITE_SECTOR(UINT8X *datbuf,UINT32 sector_num)
{
    UINT8X  s;
    UINT8X res_r0;
    if(sd_info.type!=3)
    {
        sector_num = sector_num<<9;    //����512��
    }
    param[0]=(UINT8)(sector_num>>24&0xff);
    param[1]=(UINT8)(sector_num>>16&0xff);
    param[2]=(UINT8)(sector_num>>8&0xff);
    param[3]=(UINT8)(sector_num&0xff);
    SCS = 0;
    s = SD_SendCmd(CMD24,param,&res_r0);
    if(s!=SD_ERR_CMD_OK)
    {
        SCS =1;
        return s;
    }
    SPI_TransByte(0xFF);                                                       //���ٷ�һ���������ݣ��ṩʱ�ӣ�ʹ��׼����
    SPI_TransByte(0xFE);
    for(tr=0; tr!=32; tr++)
    {
        SPI_TransByte(*datbuf++);
        SPI_TransByte(*datbuf++);
        SPI_TransByte(*datbuf++);
        SPI_TransByte(*datbuf++);
        SPI_TransByte(*datbuf++);
        SPI_TransByte(*datbuf++);
        SPI_TransByte(*datbuf++);
        SPI_TransByte(*datbuf++);
        SPI_TransByte(*datbuf++);
        SPI_TransByte(*datbuf++);
        SPI_TransByte(*datbuf++);
        SPI_TransByte(*datbuf++);
        SPI_TransByte(*datbuf++);
        SPI_TransByte(*datbuf++);
        SPI_TransByte(*datbuf++);
        SPI_TransByte(*datbuf++);
    }
    SPI_TransByte(0xFF);                                                       //�����ֽڵ�CRC
    SPI_TransByte(0xFF);
    s = SPI_TransByte(0xFF);                                                   //Data Resp,��ͬ��CMD Resp  XXX0(010)1:Accep  XXX0(101)1:CRC ERR  XXX0(110)1:WRITE ERR
    if((s&0x1F)!=5)                                                            //����д�벻��ȷ
    {
        SCS = 1;
        return s;
    }
    tr=0;
    do                                                                         //æ�ȴ�
    {
        s = SPI_TransByte(0xFF);
        tr++;
    }
    while(( s !=0xFF ) && ( tr < SD_CMD_TIMEOUT ));
    if(tr>=SD_CMD_TIMEOUT)
    {
        SCS = 1;
        return (SD_ERR_CMD_TIMEOUT);                                           //��Ӧ��ʱ
    }
    SCS = 1;
    return SD_ERR_CMD_OK;
}
/*******************************************************************************
* Function Name  : SD_WRITE_SECTOR
* Description    : д������������ڡ��������������������ţ����ڡ�С������������������ַ��
*                  Ϊ��ͳһ���ú�������Ĳ���һ��Ϊ�����ţ������ڷ����Ǵ󿨻���С����
* Input          : sector_num ������
*                  datbuf ָ��д�뻺����
*                  blocks д�����
* Output         : None
* Return         : SD_ERR_CMD_OK �ɹ��� ����
*******************************************************************************/
UINT8 SD_WRITE_MULT_SECTOR( UINT8X *datbuf,UINT32 sector_num,UINT16 blocks )
{
    UINT8X  s;
    UINT8X res_r0;
    if(sd_info.type!=3)
    {
        sector_num = sector_num<<9;    //����512��
    }
    param[0]=(UINT8)(sector_num>>24&0xff);
    param[1]=(UINT8)(sector_num>>16&0xff);
    param[2]=(UINT8)(sector_num>>8&0xff);
    param[3]=(UINT8)(sector_num&0xff);
    SCS = 0;
    s = SD_SendCmd(CMD25,param,&res_r0);
    if(s!=SD_ERR_CMD_OK)
    {
        SCS = 1;
        return s;
    }
    SPI_TransByte(0xFF);                                                       //���ٷ�һ���������ݣ��ṩʱ�ӣ�ʹ��׼����
    while(blocks--)
    {
        SPI_TransByte(0xFC);
        for(tr=0; tr!=32; tr++)
        {
            SPI_TransByte(*datbuf++);
            SPI_TransByte(*datbuf++);
            SPI_TransByte(*datbuf++);
            SPI_TransByte(*datbuf++);
            SPI_TransByte(*datbuf++);
            SPI_TransByte(*datbuf++);
            SPI_TransByte(*datbuf++);
            SPI_TransByte(*datbuf++);
            SPI_TransByte(*datbuf++);
            SPI_TransByte(*datbuf++);
            SPI_TransByte(*datbuf++);
            SPI_TransByte(*datbuf++);
            SPI_TransByte(*datbuf++);
            SPI_TransByte(*datbuf++);
            SPI_TransByte(*datbuf++);
            SPI_TransByte(*datbuf++);
        }
        SPI_TransByte(0xFF);                                                   //�����ֽڵ�CRC
        SPI_TransByte(0xFF);
        s = SPI_TransByte(0xFF);                                               //Data Resp,��ͬ��CMD Resp  XXX0(010)1:Accep  XXX0(101)1:CRC ERR  XXX0(110)1:WRITE ERR
        if((s&0x1F)!=5)                                                        //����д�벻��ȷ
        {
            SCS = 1;
            return s;
        }
        tr=0;
        do                                                                     //æ�ȴ�
        {
            s = SPI_TransByte(0xFF);
            tr++;
        }
        while(( s !=0xFF ) && ( tr < SD_CMD_TIMEOUT ));
        if(tr>=SD_CMD_TIMEOUT)
        {
            SCS = 1;
            return (SD_ERR_CMD_TIMEOUT);                                       //��Ӧ��ʱ
        }
    }
    SPI_TransByte(0xFD);                                                       //ֹͣ����
    SPI_TransByte(0xFF);                                                       //�෢8��ʱ��
    tr=0;
    do                                                                         //æ�ȴ�
    {
        s = SPI_TransByte(0xFF);
        tr++;
    }
    while(( s !=0xFF ) && ( tr < SD_CMD_TIMEOUT ));
    if(tr>=SD_CMD_TIMEOUT)
    {
        SCS = 1;
        return (SD_ERR_CMD_TIMEOUT);                                           //��Ӧ��ʱ
    }
    SCS = 1;
    return SD_ERR_CMD_OK;
}