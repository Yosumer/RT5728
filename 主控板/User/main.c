#include "Include.h"
RT_Flag Flag;

void ControlParamProcess(void);
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

void ControlParamProcess(void)
{
}