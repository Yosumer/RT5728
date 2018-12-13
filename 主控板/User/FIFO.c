#include "Include.h"
RT_FIFO FIFO;
__xdata char Buffer;
/*******************************************************************************
*       FIFO_Write
*******************************************************************************/
void FIFO_Write(char Data)
{
    if(FIFO.Lenght >= RINGBUFF_LEN) //Determines whether the buffer is full.
    {
      FIFO.Head = (FIFO.Head+1)%RINGBUFF_LEN;//Preventing cross boundary illegal access
      FIFO.Lenght--;
    }
    FIFO.Ring_Buff[FIFO.Tail]=Data;
    FIFO.Tail = (FIFO.Tail+1)%RINGBUFF_LEN;//Preventing cross boundary illegal access
    FIFO.Lenght++;
    return;
}
/*******************************************************************************
*       FIFO_Read
*******************************************************************************/
void FIFO_Read(void)
{
    if(FIFO.Lenght == 0)//Judgment is not empty
    {
        Flag.Bit.FIFO_Read=0;
        return ;
    }
    Buffer = FIFO.Ring_Buff[FIFO.Head];//FIFO, from the buffer head out
    FIFO.Head = (FIFO.Head+1)%RINGBUFF_LEN;//Preventing cross boundary illegal access
    FIFO.Lenght--;
    Flag.Bit.FIFO_Read=1;
    return;
}