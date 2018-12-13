#ifndef _VARIABLE_H_
#define _VARIABLE_H_
/*******************************************************************************
*       Flag union
*******************************************************************************/
typedef union
{
    unsigned char nCharge;
    struct
    {
        unsigned char run :1;
        unsigned char FIFO_Read :1;
        unsigned char Allow_processing :1;
        unsigned char RXC :1;
        unsigned char Allow_send :1;
        unsigned char TXBF :1;
        unsigned char Send_Count :2;
    }Bit;
}RT_Flag;
/*******************************************************************************
*       FIFO struct
*******************************************************************************/
#define RINGBUFF_LEN 0x39
typedef struct
{
    char Head;           
    char Tail;
    char Lenght;
    char Ring_Buff[RINGBUFF_LEN];
}RT_FIFO;
/*******************************************************************************
*       SendDrive struct
*******************************************************************************/
typedef union
{
    char nSendDrive[13];
    struct
    {
        char SOIA;
        char SOIB;
        char Aim_add;
        char Source_add;
        char ID;
        char CommanL;
        char CommanH;
        char LenghtL;
        char LenghtH;
        char Data[2];
        char CRCL;
        char CRCH;
    }Byte;
}RT_SendDrive;
/*******************************************************************************
*       SendHandController struct
*******************************************************************************/
typedef union
{
    char nHandController[19];
    struct
    {
        char SOIA;
        char SOIB;
        char Aim_add;
        char Source_add;
        char ID;
        char CommanL;
        char CommanH;
        char LenghtL;
        char LenghtH;
        char Data[8];
        char CRCL;
        char CRCH;
    }Byte;
}RT_HandController;
/*******************************************************************************
*       FIFO.c Function declaration
*******************************************************************************/
extern RT_FIFO FIFO;
extern __xdata char Buffer;
/*******************************************************************************
*       main.c Function declaration
*******************************************************************************/
extern RT_Flag Flag;
/*******************************************************************************
*       Comm.c Function declaration
*******************************************************************************/
extern RT_SendDrive SendDrive;
extern RT_HandController HandController;
extern __xdata char STate;
extern __xdata char CommRXCTime;
extern __xdata char CommTXCTime;
extern __xdata char TXCount;

#endif
