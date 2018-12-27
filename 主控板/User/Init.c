#include "Include.h"
#define PBS     115200
/*******************************************************************************
*       System Clock 16.6MH
*******************************************************************************/
void MODIFY_HIRC_16600(void)
{
    unsigned char hircmap0,hircmap1;
    unsigned int  trimvalue16bit;
    set_IAPEN;
    IAPAL = 0x30;
    IAPAH = 0x00;
    IAPCN = READ_UID;
    set_IAPGO;
    hircmap0 = IAPFD;
    IAPAL = 0x31;
    IAPAH = 0x00;
    set_IAPGO;
    hircmap1 = IAPFD;
    clr_IAPEN;
    trimvalue16bit = ((hircmap0<<1)+(hircmap1&0x01));
    trimvalue16bit = trimvalue16bit - 15;
    hircmap1 = trimvalue16bit&0x01;
    hircmap0 = trimvalue16bit>>1;
    TA=0XAA;
    TA=0X55;
    RCTRIM0 = hircmap0;
    TA=0XAA;
    TA=0X55;
    RCTRIM1 = hircmap1;
    return;
}
/*******************************************************************************
*       Wdcon_Init
*******************************************************************************/
void Wdcon_Init(void)
{
    /*watchdog*/ //    set_WDCLR;
    Program_CONFIG(0x04,0x5f);
    EA=0;
    TA=0xAA;
    TA=0x55;
    WDCON = 0xC6 ;//1100 0110   819.20ms
    return;
}
/*******************************************************************************
*       Time1_Init
*******************************************************************************/
void Time1_Init(void)
{
    /*Time_1 1ms*/
    TIMER1_MODE1_ENABLE;
    clr_T1M;
    clr_TR1;
    TL1 = (65536 - T1Nuber)%256; 
    TH1 = (65536 - T1Nuber)/256;        
    set_ET1;                                    //enable Timer1 interrupt
    set_TR1;                                    //Timer1 run
    return;
}
/*******************************************************************************
*       Uart0_Init
*******************************************************************************/
void Uart0_Init(void)
{
    /*UART0_Timer3 9600pbs*/
    P06_Quasi_Mode;		//Setting UART pin as Quasi mode for transmit
    P07_Quasi_Mode;		//Setting UART pin as Quasi mode for transmit	
    P06=1;
    P07=1; 
    SCON = 0x50;     //UART0 Mode1,REN=1,TI=1
    set_SMOD;        //UART0 Double Rate Enable
    T3CON &= 0xF8;   //T3PS2=0,T3PS1=0,T3PS0=0(Prescale=1)
    set_BRCK;        //UART0 baud rate clock source = Timer3
    RH3    = HIBYTE(65536 - (1037500/PBS)); 			/*16.6 MHz */
    RL3    = LOBYTE(65536 - (1037500/PBS)); 			/*16.6 MHz */
    set_TR3;         //Trigger Timer3
    ES=1;
    return;
}
/*******************************************************************************
*       PWM_Init
*******************************************************************************/
void PWM_Init(void)
{
    return;
}
/*******************************************************************************
*       GPIO_Init
*******************************************************************************/
void GPIO_Init(void)
{
    Set_All_GPIO_Quasi_Mode;
    P05_PushPull_Mode;//485EN
//    P06_PushPull_Mode;//TX
    return;
}
/*******************************************************************************
*       GPIO_Init
*******************************************************************************/
void FIFO_Init(void)
{
    FIFO.Head=0x00;
    FIFO.Tail=0x00;
    FIFO.Lenght=0x00;
    return;
}
/*******************************************************************************
*       Sys_Init
*******************************************************************************/
void Sys_Init(void)
{
    CKCON&=0xfd;
    PCON&=0xef;
    TA=0XAA;
    TA=0X55;
    BODCON0&=0xfe;
    AUXR1&=0x3f;
 //   AUXR1&=0x7f;
    CKDIV = 0x00;
    
    MODIFY_HIRC_16600();
    Wdcon_Init();
    Time1_Init();
    Uart0_Init();
    GPIO_Init();
    FIFO_Init();
    EA=1;
    return;
}