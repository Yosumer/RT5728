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
*       Feedback struct
*******************************************************************************/
typedef union
{
    char nFeedback[8];
    struct
    {
        unsigned char Auto :3;
        unsigned char Time :2;
        unsigned char Sys :2;
        unsigned char Nop1 :1;
        
        unsigned char UpKnead :2;
        unsigned char DownKnead :2;
        unsigned char Percussion :2;
        unsigned char Shock :2;
        
        unsigned char AirbagStrength :3;
        unsigned char GasbagLocation :3;
        unsigned char Nop3 :2;
        
        unsigned char heat :2;
        unsigned char heatLocation :2;
        unsigned char Nop4 :4;
        
        unsigned char Location :2;
        unsigned char UpWalk :2;
        unsigned char DownWalk :2;
        unsigned char LegWalk :2;
        
        unsigned char Up3D :2;
        unsigned char Down3D :3;
        unsigned char Nop6 :2;
        
        unsigned char UpLimitUp :1;
        unsigned char UpLimitDown :1;       
        unsigned char DownLimitUp :1;
        unsigned char DownLimitDown :1;
        unsigned char LegLimitUp :1;
        unsigned char LegLimitDown :1;
        unsigned char Nop7 :2;
        
        unsigned char UpError :2;
        unsigned char DownError :2;
        unsigned char Nop8 :4;
        
    }Byte;
}RT_Feedback;

typedef union
{
    struct
    {
        unsigned bD0:1 ;
        unsigned bD1:1 ;
        unsigned bD2:1 ;
        unsigned bD3:1 ;
        unsigned bD4:1 ;
        unsigned bD5:1 ;
        unsigned bD6:1 ;
        unsigned bD7:1 ;
    } ;
    unsigned char nByte ;
}BITS ;

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

extern __xdata char HandControlParam1;
extern __xdata char HandControlParam2;
extern __xdata char UpControlParam1;
extern __xdata char UpControlParam2;
extern __xdata char DownControlParam1;
extern __xdata char DownControlParam2;


#endif
