#include "Include.h"
RT_Flag Flag;
/*******************************************************************************
*       Main Function
*******************************************************************************/
void main (void)
{
    Sys_Init();
    RS485_EN=0;//Read
    STate=WSTART;
    while(1)
    {
        set_WDCLR;
        Comm();
    }
}