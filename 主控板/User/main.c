#include "Include.h"
RT_Flag Flag;
/*******************************************************************************
*       Main Function
*******************************************************************************/
void main (void)
{
    Sys_Init();
    //Blue  =1;
    //Red  =1;
    //Green  =1;
    RS485_EN=0;//Read
    STate=WSTART;
    while(1)
    {
        set_WDCLR;
        Comm();
    }
}