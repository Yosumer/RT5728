#include "Include.h"
RT_SendDrive SendDrive;
RT_HandController HandController;
RT_Feedback Feedback;

__xdata char CRCH;
__xdata char CRCL;
__xdata char RXCount;
__xdata char TXCount;
__xdata char STate;
__xdata char CommRXCTime;
__xdata char CommTXCTime;
__xdata char HandControlParam1;
__xdata char HandControlParam2;

char Receive[19];

void * my_memcpy(void *dest,void *src,unsigned count );

/*******************************************************************************
*       CommTime Function
*******************************************************************************/
void RXC_Time(void)
{
    if(CommRXCTime++>0x04)
    {
        Flag.Bit.RXC=1;
        CommRXCTime=0x0a;
    }
    return;
}
/*******************************************************************************
*       CommTime Function
*******************************************************************************/
void TXC_Time(void)
{
    if(CommTXCTime++>0x01)
    {
        CommTXCTime=0x0a;
        RS485_EN=0;//Read
    }
    return;
}
/*******************************************************************************
*       SendTime Function
*******************************************************************************/
void Send_Time(void)
{
    static char SendTime75ms=0;
    if(SendTime75ms++>74)
    {
        SendTime75ms=0;
        if(CommTXCTime>0x05)
        {
            Flag.Bit.Allow_send=1;
        }
    }
    return;
}
/*******************************************************************************
*       CRC_CCITT Function
*******************************************************************************/
void CRC_CCITT(char *ptr, int count)  
{  
    int crc;  
    char i;    
    crc = 0xffff;  
    while (--count >= 0)  
    {  
        crc = crc ^ (int) *ptr++ << 8;  
        i = 8;  
        do  
        {  
            if (crc & 0x8000)  
                crc = crc << 1 ^ 0x1021;  
            else  
                crc = crc << 1;  
        } while (--i);  
    }  
    CRCH= (crc>>8)&0x00ff;
    CRCL= crc&0x00ff;
    return;
} 
/*******************************************************************************
*       DataReceive Function
*******************************************************************************/
void DataReceive(void)
{
    if(Flag.Bit.FIFO_Read)
    {
        Flag.Bit.FIFO_Read=0;
        switch(STate)
        {
            case WSTART:
            {
                if(Buffer==0x55)
                {
                    Receive[0]=0x55;
                    STate=START;
                }
                break;
            }
            case START:
            {
                if(Buffer==0xaa)
                {
                    Receive[1]=0xaa;
                    RXCount=0x02;
                    STate=DATA;
                }
                break;
            }
            case DATA:
            {
                Receive[RXCount]=Buffer;
                RXCount++;
                if(RXCount>8)
                {
                    switch(Receive[7])
                    {
                        case 0x02:
                        {
                            if(RXCount==0x0d)
                            {
                                STate=WSTART;
                                Flag.Bit.Allow_processing=1;
                            }
                            break;
                        }
                        case 0x08:
                        {
                            if(RXCount==0x13)
                            {
                                STate=WSTART;
                                Flag.Bit.Allow_processing=1;
                            }
                            break;
                        }
                    }
                }
                break;
            }
        }
    }
    return;
}
/*******************************************************************************
*       Communication Processing Function
*******************************************************************************/
void CommProcess(void)
{
    if(Flag.Bit.Allow_processing)
    {
        Flag.Bit.Allow_processing=0;
        switch(RXCount)
        {
            case 0x0d:
            {
                CRC_CCITT(Receive,11);//扶手按键
                if((Receive[11]==CRCL)&&(Receive[12]==CRCH))
                {
                    if(Receive[2]!=0x0a) break;
                    HandControlParam1 = Receive[9];
                    HandControlParam2 = Receive[10];
                }
                break;
            }
            case 0x13:
            {
                CRC_CCITT(Receive,17);//机芯
                if((Receive[17]==CRCL)&&(Receive[18]==CRCH))
                {
                    switch(Receive[2])
                    {
                        case 0x06://上机芯
                            Feedback.Byte.UpKnead = Receive[10]&0x3;
                            Feedback.Byte.Percussion = (Receive[10]&0x30)>>4;
                            Feedback.Byte.UpWalk = (Receive[13]&0xc)>>2;
                            Feedback.Byte.UpLimitUp = Receive[15]&0x1;
                            Feedback.Byte.UpLimitDown = (Receive[15]&0x2)>>1;
                            Feedback.Byte.UpError = Receive[16]&0x3;
                            break;
                        case 0x07://下机芯
                            Feedback.Byte.DownKnead = (Receive[10]&0xc)>>2;
                            Feedback.Byte.Percussion = (Receive[10]&0x30)>>4;
                            Feedback.Byte.DownWalk = (Receive[13]&0x30)>>4;
                            Feedback.Byte.DownLimitUp = (Receive[15]&0x4)>>2;
                            Feedback.Byte.DownLimitDown = (Receive[15]&0x8)>>3;
                            Feedback.Byte.DownError = (Receive[16]&0xc)>>2;
                            break;
                        default:
                            break;
                    }
                }
                break;
            }
            default:
            {
                break;
            }
        }
    }
    return;
}
/*******************************************************************************
*       CommSend Processing Function
*******************************************************************************/
void CommSend(void)
{
    if(Flag.Bit.RXC)
    {
        if(Flag.Bit.Allow_send)
        {
            Flag.Bit.Allow_send=0;
            Flag.Bit.Send_Count++;
            switch(Flag.Bit.Send_Count)
            {
                case 0x01://100#
                {
                    SendDrive.Byte.SOIA=0x55;
                    SendDrive.Byte.SOIB=0xAA;
                    SendDrive.Byte.Aim_add=0x06;
                    SendDrive.Byte.Source_add=0x01;
                    SendDrive.Byte.ID=0x00;
                    SendDrive.Byte.CommanL=0x05;
                    SendDrive.Byte.CommanH=0x30;
                    SendDrive.Byte.LenghtL=0x02;
                    SendDrive.Byte.LenghtH=0x00;
                    SendDrive.Byte.Data[0]=HandControlParam1;
                    SendDrive.Byte.Data[1]=HandControlParam2;
                    CRC_CCITT(SendDrive.nSendDrive,11);
                    SendDrive.Byte.CRCL=CRCL;
                    SendDrive.Byte.CRCH=CRCH;
                    TXCount=0x00;
                    Flag.Bit.TXBF=1;
                    TI=1;
                    break;
                }
                case 0x03://110#
                {
                    SendDrive.Byte.SOIA=0x55;
                    SendDrive.Byte.SOIB=0xAA;
                    SendDrive.Byte.Aim_add=0x07;
                    SendDrive.Byte.Source_add=0x01;
                    SendDrive.Byte.ID=0x00;
                    SendDrive.Byte.CommanL=0x05;
                    SendDrive.Byte.CommanH=0x30;
                    SendDrive.Byte.LenghtL=0x02;
                    SendDrive.Byte.LenghtH=0x00;
                    SendDrive.Byte.Data[0]=HandControlParam1;
                    SendDrive.Byte.Data[1]=HandControlParam2;
                    CRC_CCITT(SendDrive.nSendDrive,11);
                    SendDrive.Byte.CRCL=CRCL;
                    SendDrive.Byte.CRCH=CRCH;
                    TXCount=0x00;
                    Flag.Bit.TXBF=1;
                    TI=1;
                    break;
                }
                default://HandController
                {
                    HandController.Byte.SOIA=0x55;
                    HandController.Byte.SOIB=0xAA;
                    HandController.Byte.Aim_add=0x0a;
                    HandController.Byte.Source_add=0x01;
                    HandController.Byte.ID=0x00;
                    HandController.Byte.CommanL=0x04;
                    HandController.Byte.CommanH=0x30;
                    HandController.Byte.LenghtL=0x08;
                    HandController.Byte.LenghtH=0x00;
                    my_memcpy(HandController.Byte.Data,&Feedback,sizeof(Feedback));
                    CRC_CCITT(HandController.nHandController,17);
                    HandController.Byte.CRCL=CRCL;
                    HandController.Byte.CRCH=CRCH;
                    TXCount=0x00;
                    Flag.Bit.TXBF=1;
                    TI=1;
                    break;
                }
            }         
        }
    }
    return;
}

/*******************************************************************************
*       my_memcpy Processing Function
*******************************************************************************/
void * my_memcpy(void *dest,void *src,unsigned count )
{
	if (dest == NULL || src == NULL)
	{
		return NULL;
	}
	char* pdest =(char*) dest;
	char* psrc = (char*)src;
	while (count--)
	{
		*pdest++ = *psrc++;
	}
	return dest;
}

void Comm(void)
{
    FIFO_Read();
    DataReceive();
    CommProcess();
    CommSend();
    return;
}
