#include "Include.h"
RT_Flag Flag;
extern RT_Feedback Feedback;
/*******************************************************************************
*       Main Function
*******************************************************************************/
void All_PowrOn(void)
{
    Power = 1;
    Feedback.Byte.Sys = 1;
}

void All_PowrOff(void)
{
    Power = 0;
    Hot = 0;
    Feedback.Byte.Sys = 0;
    Feedback.Byte.heat = 0;
}

void Main_Vibrate_Control(void)
{

}

void Main_Sys(void)
{
    switch(HandControlParam2)
    {
        case 0x00:  //空键值
            break;
        case 0x01:  //电源开
            All_PowrOn();
            break;
        case 0x07:  //电源关
            All_PowrOff();
            break;
        case 0x05:  //暂停
            break;
        case 0x06:  //暂停开启
            break;
        default:
            break;
    }
}

void Main_Update(void)
{

}

void Main_Start_Auto(void)
{

}

void Main_Start_Manual(void)
{
    switch(HandControlParam2)
    {
        case 0x60:
        case 0x61:
        case 0x62:
        case 0x63:
        case 0x64:
        case 0x65:
        case 0x66:
            UpControlParam1 = HandControlParam1;
            UpControlParam2 = HandControlParam2;
            break;
        case 0x67:
        case 0x68:
        case 0x69:
        case 0x6A:
        case 0x6B:
        case 0x6C:
        case 0x6D:
            DownControlParam1 = HandControlParam1;
            DownControlParam2 = HandControlParam2;
            break;
        case 0x75:
        case 0x76:
            
        case 0x77:
        case 0x78:
        default:
            break;
    }
}

void Main_ChairParamSet(void)
{
    switch(HandControlParam2)
    {
        case 0x10://下机芯
        case 0x11:
        case 0x12:
        case 0x13:
            DownControlParam1 = HandControlParam1;
            DownControlParam2 = HandControlParam2;
            break;
        case 0x1D://上机芯
        case 0x1E:
        case 0x1F:
            UpControlParam1 = HandControlParam1;
            UpControlParam2 = HandControlParam2;
            break;
    }
}

void Main_HeatPro()
{
    switch(HandControlParam2)
    {
        case 0x18://加热背部关
            if(Hot)
                Hot = 0;
            break;
        case 0x19://加热背部1
            if(!Hot&&Power)
                Hot = 1;
            break;
    }
    Feedback.Byte.heat=(unsigned char)Hot;
    Feedback.Byte.heatLocation=3;
}

void ControlParamProcess(void)
{
    switch(HandControlParam1)
    {
        case 0x00://系统控制
            Main_Sys();
            break;
        case 0x02://升级
            Main_Update();
            break;
        case 0x10://自动程序
            Main_Start_Auto();
            break;
        case 0x20://手动程序
            Main_Start_Manual();
            break;
        case 0x40://辅助按摩程序 加热程序
            Main_HeatPro();
            break;
        case 0x50://按摩椅参数调整
            Main_ChairParamSet();
            break;
        default:
            break;
    }
}
void main (void)
{
    Sys_Init();
    RS485_EN=0;//Read
    Power=0;
    Hot=0;
    STate=WSTART;
    while(1)
    {
        set_WDCLR;
        Comm();
        ControlParamProcess();
    }
}