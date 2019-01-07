#include "Include.h"
RT_Flag Flag;

/*******************************************************************************
*       
*******************************************************************************/

unsigned char nChairRunState;



/*******************************************************************************
*       
*******************************************************************************/
void Main_Settle(void);
void Sleep_Process(void);
void Main_WaitCommand(void);
void Main_Work_Run(void);







/*******************************************************************************
*      Function
*******************************************************************************/
void Main_Settle(void)
{

}

void Sleep_Process(void)
{

}

void Main_WaitCommand(void)
{

}

void Main_Work_Run(void)
{

}

void main (void)
{
    Sys_Init();
    RS485_EN=0;//Read
    STate=WSTART;
    while(1)
    {
        set_WDCLR;
        Comm();
        switch (nChairRunState)
        {
            case CHAIR_STATE_SETTLE:
                Main_Settle();
                break;
            case CHAIR_STATE_SLEEP:
                Sleep_Process();
                break;
            case CHAIR_STATE_WAIT_COMMAND:
                Main_WaitCommand();
                break;
            case CHAIR_STATE_RUN:
                Main_Work_Run();
                break;
            default:
                break;
        }
    }
}
