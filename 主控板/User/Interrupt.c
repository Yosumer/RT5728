#include "Include.h"
/*******************************************************************************
*       Time_1 interrupt add 1BH(1ms)
*******************************************************************************/
#pragma vector = 0x1b
__interrupt void Timer1_ISR (void)      //interrupt address is 0x001B
{
    TR1 = 0; 
    TL1 = ((65536 - T1Nuber)%256)+TL1; 
    TH1 = (65536 - T1Nuber)/256;
    TR1 = 1;
    RXC_Time();
    TXC_Time();
    Send_Time();
//    LED=~LED;
}
/*******************************************************************************
*       UART0 interrupt add 23H 
*******************************************************************************/
#pragma vector = 0x23
__interrupt void UART0 (void)      //interrupt address is 0x0023
{
    TI=0;
    if(Flag.Bit.TXBF)
    {
        CommTXCTime=0x00;
        RS485_EN=1;//Write;
        switch(Flag.Bit.Send_Count%2)
        {
            case 0x00://HandController
            {
                switch(TXCount)
                {
                    case 0x00:
                    {
                        Send_Data_To_UART0(HandController.nHandController[TXCount]);
                        TXCount++;
                        break;
                    }
                    case 0x13:
                    {
                        Flag.Bit.TXBF=0;
                        break;
                    }
                    default:
                    {
                        Send_Data_To_UART0(HandController.nHandController[TXCount]);
                        TXCount++;
                        break;
                    }
                }
                break;
            }
            case 0x01://SendDrive
            {
                switch(TXCount)
                {
                    case 0x00:
                    {
                        Send_Data_To_UART0(SendDrive.nSendDrive[TXCount]);
                        TXCount++;
                        break;
                    }
                    case 0x0d:
                    {
                        Flag.Bit.TXBF=0;
                        break;
                    }
                    default:
                    {
                        Send_Data_To_UART0(SendDrive.nSendDrive[TXCount]);
                        TXCount++;
                        break;
                    }
                }
                break;
            }
            default:
            {
                Flag.Bit.TXBF=0;
                break;
            }
        }
    }
    if(RI)
    {
        RI=0;
        Flag.Bit.RXC=0;
        CommRXCTime=0x00;
        FIFO_Write(SBUF);
    }
}