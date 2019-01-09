#include "Include.h"
/*******************************************************************************/
RT_Flag Flag;
extern RT_Feedback Feedback;

__xdata unsigned char nChairRunState;
__xdata unsigned char nBackMainRunMode;
__xdata unsigned char nMaunalSubMode;
__xdata unsigned char nBackSubRunMode;
__xdata unsigned char Auto_init_Enable;

__xdata unsigned char Settle_State;
__xdata unsigned int w_Timer_Sleep;
__xdata unsigned int w_Timer_Wait;
__xdata unsigned int Run_Time;
__xdata unsigned int UpWalkControlTime;
__xdata unsigned int DownWalkControlTime;


__no_init BITS GlobalFlags0;
#define bKeyBackPadUp           GlobalFlags0.bD0
#define bKeyBackPadDown         GlobalFlags0.bD1
#define bKeyBackPadHot          GlobalFlags0.bD2
#define bKeyLegVibrate          GlobalFlags0.bD3
#define bKeyAutoInit            GlobalFlags0.bD4
#define bKeyNextStep            GlobalFlags0.bD5
#define bKeyUpWorkOk            GlobalFlags0.bD6
#define bKeyDownWorkOk          GlobalFlags0.bD7

__no_init Motor_Struct AutoDirector;
__no_init unsigned int nCurActionStep ;
__no_init unsigned int nMaxActionStep ;


/*******************************************************************************/
void PowerOn_24V(void)
{
    Power = 1;
    Feedback.Byte.Sys = 1;
}

void PowerOff_24V(void)
{
    Power = 0;
    Hot = 0;
    Vibrate = 0;
    Feedback.Byte.heat = 0;
    Feedback.Byte.Sys = 0;
    Feedback.Byte.Shock = 0;
    Feedback.Byte.heatLocation = 0;
}

void Hot_On(void)
{
    Hot = 1;
    Feedback.Byte.heat = 1;
    Feedback.Byte.heatLocation = 1;
}

void Hot_Off(void)
{
    Hot = 0;
    Feedback.Byte.heat = 0;
    Feedback.Byte.heatLocation = 0;
}

void Vibrate_On(void)
{
    Vibrate = 1;
    Feedback.Byte.Shock = 1;
}

void Vibrate_Off(void)
{
    Vibrate = 0;
    Feedback.Byte.Shock = 0;
}

void Data_Init(void)
{
    RS485_EN=0;//Read
    STate=WSTART;
}

void Time1s_pro(void)
{
    if(nChairRunState == CHAIR_STATE_SLEEP)
    w_Timer_Sleep++;
    if(nChairRunState == CHAIR_STATE_WAIT_COMMAND)
    w_Timer_Wait++;

    if(Run_Time)
        Run_Time--;
}

void Time200ms_pro(void)
{
    if(UpWalkControlTime)
        UpWalkControlTime--;
    if(DownWalkControlTime)
        DownWalkControlTime--;
}


unsigned int Get_RstTime(void)
{
    return Run_Time;
}

unsigned char Uart_GetKey(void)
{
    unsigned char key;
    switch(HandControlParam1)
    {
        case 0x00:
            if(HandControlParam2==0x00)
                key = H10_KEY_NONE;
            else if(HandControlParam2==0x01)
                key = H10_KEY_POWER_ON;
            else if(HandControlParam2==0x07)
                key = H10_KEY_POWER_OFF;
            break;
        case 0x02:
            if(HandControlParam2==0x10)
                key = H10_KEY_UPDATE;
            break;
        case 0x10://自动程序
            key = HandControlParam2;
            break;
        case 0x20://手动程序
            switch(HandControlParam2)
            {
                case 0x60:
                    key = H10_KEY_UP_KNEAD_OFF;
                    break;
                case 0x61:
                    key = H10_KEY_UP_KNEAD_UP;
                    break;
                case 0x62:
                    key = H10_KEY_UP_KNEAD_DOWN;
                    break;
                case 0x67:
                    key = H10_KEY_DOWN_KNEAD_OFF;
                    break;
                case 0x68:
                    key = H10_KEY_DOWN_KNEAD_UP;
                    break;
                case 0x69:
                    key = H10_KEY_DOWN_KNEAD_DOWN;
                    break;
                case 0x6a:
                    key = H10_KEY_DOWN_KNOCK_OFF;
                    break;
                case 0x6b:
                    key = H10_KEY_DOWN_KNOCK_ON;
                    break;
                case 0x75:
                    key = H10_KEY_LEG_VIBRATE_OFF;
                    break;
                case 0x76:
                    key = H10_KEY_LEG_VIBRATE_ON;
                    break;
            }
            break;
        case 0x40://加热
            if(HandControlParam2==0x18)
                key = H10_KEY_HEAT_OFF;
            else if(HandControlParam2==0x19)
                key = H10_KEY_HEAT_ON;
            break;
        case 0x50:
            switch(HandControlParam2)
            {
                case 0x10:
                    key = H10_KEY_DOWNSIDE_UP;
                    break;
                case 0x11:
                    key = H10_KEY_DOWNSIDE_UP_STOP;
                    break;
                case 0x12:
                    key = H10_KEY_DOWNSIDE_DOWN;
                    break;
                case 0x13:
                    key = H10_KEY_DOWNSIDE_DOWN_STOP;
                    break;
                case 0x14:
                    key = H10_KEY_BACKPAD_UP;
                    break;
                case 0x15:
                    key = H10_KEY_BACKPAD_DOWN;
                    break;
                case 0x16:
                    key = H10_KEY_BACKPAD_STOP;
                    break;
                case 0x1D:
                    key = H10_KEY_UPSIDE_UP;
                    break;
                case 0x1E:
                    key = H10_KEY_UPSIDE_DOWN;
                    break;
                case 0x1F:
                    key = H10_KEY_UPSIDE_STOP;
                    break;
            }
            break;
    }
    return key;
}


//靠背电动缸控制程序
void Main_BackPad_Proce(void)
{
    if(bKeyBackPadUp == True)
    {
        if(BackMotor_Control(STATE_RUN_BACK_UP))
            Feedback.Byte.Location = 1;
    }
    else if(bKeyBackPadDown == True)
    {
        if(BackMotor_Control(STATE_RUN_BACK_DOWN))
            Feedback.Byte.Location = 2;
    }
    else 
        if(BackMotor_Control(STATE_BACK_IDLE))
            Feedback.Byte.Location = 0;
}

void Main_Start_Manual(void)
{
    switch(nMaunalSubMode)
    {
        case nMaunalSubMode_UP:
            UpControlParam1 = HandControlParam1;
            UpControlParam2 = HandControlParam2;
            break;
        case nMaunalSubMode_DOWN:
            DownControlParam1 = HandControlParam1;
            DownControlParam2 = HandControlParam2;
            break;
    }
}

void Main_Start_Auto(void)
{
    switch(nBackSubRunMode)
    {
        case BACK_SUB_MODE_AUTO_0:
            if(!bKeyAutoInit)
            {   
                bKeyAutoInit = True;
                #ifdef Auto_20Min
                    Run_Time = TIME_20MIN;
                #else  
                    Run_Time = TIME_10MIN;
                #endif
                nMaxActionStep = 10;
                nCurActionStep = 0;
                bKeyNextStep = False;
            }
            AutoDirector = AutoFunction0[nCurActionStep];
            break;
        case BACK_SUB_MODE_AUTO_1:

            break;
        case BACK_SUB_MODE_AUTO_2:

            break;
        case BACK_SUB_MODE_AUTO_3:

            break;
    }
}

void UpMotorControl(void)
{
    static char Up_Step = 0;
    switch(Up_Step)
    {
        case 0:
            if(AutoDirector.UpKneadMotorAction == UpKnead_Stop)
            {
                UpControlParam1 = 0x20;
                UpControlParam2 = 0x60;
                if(Feedback.Byte.UpKnead == UpKnead_Stop)
                    Up_Step=1;
            }else if(AutoDirector.UpKneadMotorAction == UpKnead_Positive)
            {
                UpControlParam1 = 0x20;
                UpControlParam2 = 0x61;
                if(Feedback.Byte.UpKnead == UpKnead_Stop)
                    Up_Step=1;
            }else if(AutoDirector.UpKneadMotorAction == UpKnead_Reverse)
            {
                UpControlParam1 = 0x20;
                UpControlParam2 = 0x62;
                if(Feedback.Byte.UpKnead == UpKnead_Reverse)
                    Up_Step=1;
            }
            break;
        case 1:
            if(AutoDirector.UpWalkMotorDir == UpWalk_LOCATE_Stop)
            {
                UpControlParam1 = 0x50;
                UpControlParam2 = 0x1F;
                if((Feedback.Byte.UpWalk == UpWalk_LOCATE_Stop)&&
                    (UpWalkControlTime==0))
                    bKeyUpWorkOk = True;
            }
            if(AutoDirector.UpWalkMotorDir == UpWalk_LOCATE_Up)
            {
                UpControlParam1 = 0x50;
                UpControlParam2 = 0x1D;
                if((Feedback.Byte.UpWalk == UpWalk_LOCATE_Up)&&
                    (UpWalkControlTime==0))
                {
                    UpControlParam1 = 0x50;
                    UpControlParam2 = 0x1F;
                    bKeyUpWorkOk = True;
                }
            }
                
    }
}

void DownMotorControl(void)
{

}

void Main_BackProce(void)
{
    if(bKeyNextStep ==True)
    {
        bKeyNextStep = False;
        nCurActionStep++;
        if(nCurActionStep>=nMaxActionStep)
            nCurActionStep = 0;
    }
    if(!UpWalkControlTime&&!DownWalkControlTime)
        bKeyNextStep = True;
    UpMotorControl();
    DownMotorControl();
}
void Main_Work_Run(void)
{
    unsigned char key;
    PowerOn_24V();

    if(nBackMainRunMode == BACK_MAIN_MODE_AUTO)
    {
        Main_Start_Auto();
    }
    else if(nBackMainRunMode == BACK_MAIN_MODE_MANUAL)
    {
        Main_Start_Manual();
    }
    key = Uart_GetKey();
    if(key!=H10_KEY_BACKPAD_UP&&key!=H10_KEY_BACKPAD_DOWN)
    {
        bKeyBackPadUp = 0;
        bKeyBackPadDown = 0;
    }
    switch(key)
    {
        case H10_KEY_POWER_OFF:
            nChairRunState = CHAIR_STATE_SETTLE;
            break;
        case H10_KEY_UPDATE:
            break;
        case H10_KEY_CHAIR_AUTO_0:
            if(nBackSubRunMode == BACK_SUB_MODE_AUTO_0) break;
            nChairRunState = CHAIR_STATE_RUN ;
            nBackMainRunMode = BACK_MAIN_MODE_AUTO;
            nBackSubRunMode = BACK_SUB_MODE_AUTO_0;
            bKeyAutoInit = False;
            break ;
        case H10_KEY_CHAIR_AUTO_1:
            if(nBackSubRunMode == BACK_SUB_MODE_AUTO_1) break;
            nChairRunState = CHAIR_STATE_RUN ;
            nBackMainRunMode = BACK_MAIN_MODE_AUTO;
            nBackSubRunMode = BACK_SUB_MODE_AUTO_1;
            bKeyAutoInit = False;
            break ;
        case H10_KEY_CHAIR_AUTO_2:
            if(nBackSubRunMode == BACK_SUB_MODE_AUTO_2) break;
            nChairRunState = CHAIR_STATE_RUN ;
            nBackMainRunMode = BACK_MAIN_MODE_AUTO;
            nBackSubRunMode = BACK_SUB_MODE_AUTO_2;
            bKeyAutoInit = False;
            break ;
        case H10_KEY_CHAIR_AUTO_3:
            if(nBackSubRunMode == BACK_SUB_MODE_AUTO_3) break;
            nChairRunState = CHAIR_STATE_RUN ;
            nBackMainRunMode = BACK_MAIN_MODE_AUTO;
            nBackSubRunMode = BACK_SUB_MODE_AUTO_3;
            bKeyAutoInit = False;
            break ;
        case H10_KEY_UP_KNEAD_OFF:
            nChairRunState = CHAIR_STATE_RUN ;
            nBackMainRunMode = BACK_MAIN_MODE_MANUAL;
            nMaunalSubMode = nMaunalSubMode_UP;
            break;
        case H10_KEY_UP_KNEAD_UP:
            nChairRunState = CHAIR_STATE_RUN ;
            nBackMainRunMode = BACK_MAIN_MODE_MANUAL;
            nMaunalSubMode = nMaunalSubMode_UP;
            break;
        case H10_KEY_UP_KNEAD_DOWN:
            nChairRunState = CHAIR_STATE_RUN ;
            nBackMainRunMode = BACK_MAIN_MODE_MANUAL;
            nMaunalSubMode = nMaunalSubMode_UP;
            break;
        case H10_KEY_DOWN_KNEAD_OFF:
            nChairRunState = CHAIR_STATE_RUN ;
            nBackMainRunMode = BACK_MAIN_MODE_MANUAL;
            nMaunalSubMode = nMaunalSubMode_DOWN;
            break;
        case H10_KEY_DOWN_KNEAD_UP:
            nChairRunState = CHAIR_STATE_RUN ;
            nBackMainRunMode = BACK_MAIN_MODE_MANUAL;
            nMaunalSubMode = nMaunalSubMode_DOWN;
            break;
        case H10_KEY_DOWN_KNEAD_DOWN:
            nChairRunState = CHAIR_STATE_RUN ;
            nBackMainRunMode = BACK_MAIN_MODE_MANUAL;
            nMaunalSubMode = nMaunalSubMode_DOWN;
            break;
        case H10_KEY_DOWN_KNOCK_OFF:
            nChairRunState = CHAIR_STATE_RUN ;
            nBackMainRunMode = BACK_MAIN_MODE_MANUAL;
            nMaunalSubMode = nMaunalSubMode_DOWN;
            break;
        case H10_KEY_DOWN_KNOCK_ON:
            nChairRunState = CHAIR_STATE_RUN ;
            nBackMainRunMode = BACK_MAIN_MODE_MANUAL;
            nMaunalSubMode = nMaunalSubMode_DOWN;
            break;
        case H10_KEY_DOWNSIDE_UP:
            nChairRunState = CHAIR_STATE_RUN ;
            nBackMainRunMode = BACK_MAIN_MODE_MANUAL;
            nMaunalSubMode = nMaunalSubMode_DOWN;
            break;
        case H10_KEY_DOWNSIDE_UP_STOP:
            nChairRunState = CHAIR_STATE_RUN ;
            nBackMainRunMode = BACK_MAIN_MODE_MANUAL;
            nMaunalSubMode = nMaunalSubMode_DOWN;
            break;
        case H10_KEY_DOWNSIDE_DOWN:
            nChairRunState = CHAIR_STATE_RUN ;
            nBackMainRunMode = BACK_MAIN_MODE_MANUAL;
            nMaunalSubMode = nMaunalSubMode_DOWN;
            break;
        case H10_KEY_DOWNSIDE_DOWN_STOP:
            nChairRunState = CHAIR_STATE_RUN ;
            nBackMainRunMode = BACK_MAIN_MODE_MANUAL;
            nMaunalSubMode = nMaunalSubMode_DOWN;
            break;
        case H10_KEY_UPSIDE_UP:
            nChairRunState = CHAIR_STATE_RUN ;
            nBackMainRunMode = BACK_MAIN_MODE_MANUAL;
            nMaunalSubMode = nMaunalSubMode_UP;
            break;
        case H10_KEY_UPSIDE_DOWN:
            nChairRunState = CHAIR_STATE_RUN ;
            nBackMainRunMode = BACK_MAIN_MODE_MANUAL;
            nMaunalSubMode = nMaunalSubMode_UP;
            break;
        case H10_KEY_UPSIDE_STOP:
            nChairRunState = CHAIR_STATE_RUN ;
            nBackMainRunMode = BACK_MAIN_MODE_MANUAL;
            nMaunalSubMode = nMaunalSubMode_UP;
            break;
        case H10_KEY_HEAT_OFF:
            nChairRunState = CHAIR_STATE_RUN ;
            bKeyBackPadHot = False;
            break;
        case H10_KEY_HEAT_ON:
            nChairRunState = CHAIR_STATE_RUN ;
            bKeyBackPadHot = True;
            break;
        case H10_KEY_BACKPAD_UP:
            nChairRunState = CHAIR_STATE_RUN ;
            bKeyBackPadUp = True;
            bKeyBackPadDown = False;
            break;
        case H10_KEY_BACKPAD_DOWN:
            nChairRunState = CHAIR_STATE_RUN ;
            bKeyBackPadUp = False;
            bKeyBackPadDown = True;
            break;
        case H10_KEY_BACKPAD_STOP:
            nChairRunState = CHAIR_STATE_RUN ;
            bKeyBackPadUp = False;
            bKeyBackPadDown = False;
            break;
        case H10_KEY_LEG_VIBRATE_ON:
            nChairRunState = CHAIR_STATE_RUN ;
            bKeyLegVibrate = True;
            break;
        case H10_KEY_LEG_VIBRATE_OFF:
            nChairRunState = CHAIR_STATE_RUN ;
            bKeyLegVibrate = False;
            break;
        default:
            break;
    }
    Main_BackPad_Proce();
    switch(nBackMainRunMode)
    {
        case BACK_MAIN_MODE_AUTO:  
            Main_BackProce();
            break;
        default:
            break;
    }
    if(bKeyBackPadHot)
        Hot_On();
    else
        Hot_Off();
    if(bKeyLegVibrate)
        Vibrate_On();
    else 
        Vibrate_Off();
}






void Main_Settle(void)//复位函数
{
    if(nChairRunState==CHAIR_STATE_SETTLE)
    {
        Settle_State = 3;
        switch(Settle_State)
        {
            case 0:
                Power = 1;
                Hot_Off();//加热关
                Vibrate_Off();//震动关
                //上下机芯揉捏关
                UpControlParam1 = 0x20;
                UpControlParam2 = 0x60;
                DownControlParam1 = 0x20;
                DownControlParam2 = 0x67;
                if((!Feedback.Byte.UpKnead)
                    &&(!Feedback.Byte.DownKnead))
                {
                    UpControlParam1 = 0;
                    UpControlParam2 = 0;
                    DownControlParam1 = 0;
                    DownControlParam2 = 0;
                    Settle_State = 1;
                }
                break;
            case 1:
                //下机芯敲关
                DownControlParam1 = 0x20;
                DownControlParam2 = 0x6a;
                if(!Feedback.Byte.Percussion)
                {
                    DownControlParam1 = 0;
                    DownControlParam2 = 0;
                    Settle_State = 2;
                }
                break;
            case 2:
                //上下机芯走到限位
                if(!Feedback.Byte.UpLimitUp)
                {
                    UpControlParam1 = 0x50;
                    UpControlParam2 = 0x10;
                }
                else
                {
                    UpControlParam1 = 0;
                    UpControlParam2 = 0;
                }
                if(!Feedback.Byte.DownLimitDown)
                {
                    DownControlParam1 = 0x50;
                    DownControlParam2 = 0x1E;
                }
                else
                {
                    DownControlParam1 = 0;
                    DownControlParam2 = 0;
                }
                if(Feedback.Byte.UpLimitUp&&Feedback.Byte.DownLimitDown)
                    Settle_State = 3;
                break;
            case 3:
                PowerOff_24V();
                nChairRunState = CHAIR_STATE_SLEEP;
                break;
            default:
                break;
        }
    }
}

void Sleep(void)
{
    if(w_Timer_Sleep>30)//30s钟后进入掉电模式
    {
        w_Timer_Sleep = 0;
        Erase_CONFIG();
        set_PD;
    }
}

void Main_Sleep_Process(void)
{
    unsigned char key;
    Sleep();
    PowerOff_24V();
    bKeyBackPadHot = 0;
    bKeyBackPadUp = 0;
    bKeyBackPadDown = 0;
    bKeyLegVibrate = 0;
    UpControlParam1 = 0;
    UpControlParam2 = 0;
    DownControlParam1 = 0;
    DownControlParam2 = 0;
    key = Uart_GetKey();
    if(key != H10_KEY_NONE)
    {
        w_Timer_Sleep=0;//睡眠定时器清0
        if(key == H10_KEY_POWER_ON)
        {
            PowerOn_24V();
            nChairRunState = CHAIR_STATE_WAIT_COMMAND;
        }
    }
}

void Main_WaitCommand(void)
{
    unsigned char key;
    if(w_Timer_Wait>29)//30s无动作进入休眠
    {
        w_Timer_Wait = 0;
        PowerOff_24V();
        nChairRunState = CHAIR_STATE_SLEEP;
        return;
    }
    key = Uart_GetKey();
    //靠背按键长按
    if(key!=H10_KEY_BACKPAD_UP&&key!=H10_KEY_BACKPAD_DOWN)
    {
        bKeyBackPadUp = 0;
        bKeyBackPadDown = 0;
    }
    
    if(H10_KEY_NONE != key)
    {
        w_Timer_Wait = 0;
        switch(key)
        {
            case H10_KEY_POWER_OFF:
                nChairRunState = CHAIR_STATE_SLEEP;
                break;
            case H10_KEY_UPDATE:
                break;
            case H10_KEY_CHAIR_AUTO_0:
                nChairRunState = CHAIR_STATE_RUN ;
                nBackMainRunMode = BACK_MAIN_MODE_AUTO;
                nBackSubRunMode = BACK_SUB_MODE_AUTO_0;
                bKeyAutoInit = False;
                break ;
            case H10_KEY_CHAIR_AUTO_1:
                nChairRunState = CHAIR_STATE_RUN ;
                nBackMainRunMode = BACK_MAIN_MODE_AUTO;
                nBackSubRunMode = BACK_SUB_MODE_AUTO_1;
                bKeyAutoInit = False;
                break ;
            case H10_KEY_CHAIR_AUTO_2:
                nChairRunState = CHAIR_STATE_RUN ;
                nBackMainRunMode = BACK_MAIN_MODE_AUTO;
                nBackSubRunMode = BACK_SUB_MODE_AUTO_2;
                bKeyAutoInit = False;
                break ;
            case H10_KEY_CHAIR_AUTO_3:
                nChairRunState = CHAIR_STATE_RUN ;
                nBackMainRunMode = BACK_MAIN_MODE_AUTO;
                nBackSubRunMode = BACK_SUB_MODE_AUTO_3;
                bKeyAutoInit = False;
                break ;
            case H10_KEY_UP_KNEAD_OFF:
                nChairRunState = CHAIR_STATE_RUN ;
                nBackMainRunMode = BACK_MAIN_MODE_MANUAL;
                nMaunalSubMode = nMaunalSubMode_UP;
                break;
            case H10_KEY_UP_KNEAD_UP:
                nChairRunState = CHAIR_STATE_RUN ;
                nBackMainRunMode = BACK_MAIN_MODE_MANUAL;
                nMaunalSubMode = nMaunalSubMode_UP;
                break;
            case H10_KEY_UP_KNEAD_DOWN:
                nChairRunState = CHAIR_STATE_RUN ;
                nBackMainRunMode = BACK_MAIN_MODE_MANUAL;
                nMaunalSubMode = nMaunalSubMode_UP;
                break;
            case H10_KEY_DOWN_KNEAD_OFF:
                nChairRunState = CHAIR_STATE_RUN ;
                nBackMainRunMode = BACK_MAIN_MODE_MANUAL;
                nMaunalSubMode = nMaunalSubMode_DOWN;
                break;
            case H10_KEY_DOWN_KNEAD_UP:
                nChairRunState = CHAIR_STATE_RUN ;
                nBackMainRunMode = BACK_MAIN_MODE_MANUAL;
                nMaunalSubMode = nMaunalSubMode_DOWN;
                break;
            case H10_KEY_DOWN_KNEAD_DOWN:
                nChairRunState = CHAIR_STATE_RUN ;
                nBackMainRunMode = BACK_MAIN_MODE_MANUAL;
                nMaunalSubMode = nMaunalSubMode_DOWN;
                break;
            case H10_KEY_DOWN_KNOCK_OFF:
                nChairRunState = CHAIR_STATE_RUN ;
                nBackMainRunMode = BACK_MAIN_MODE_MANUAL;
                nMaunalSubMode = nMaunalSubMode_DOWN;
                break;
            case H10_KEY_DOWN_KNOCK_ON:
                nChairRunState = CHAIR_STATE_RUN ;
                nBackMainRunMode = BACK_MAIN_MODE_MANUAL;
                nMaunalSubMode = nMaunalSubMode_DOWN;
                break;
            case H10_KEY_DOWNSIDE_UP:
                nChairRunState = CHAIR_STATE_RUN ;
                nBackMainRunMode = BACK_MAIN_MODE_MANUAL;
                nMaunalSubMode = nMaunalSubMode_DOWN;
                break;
            case H10_KEY_DOWNSIDE_UP_STOP:
                nChairRunState = CHAIR_STATE_RUN ;
                nBackMainRunMode = BACK_MAIN_MODE_MANUAL;
                nMaunalSubMode = nMaunalSubMode_DOWN;
                break;
            case H10_KEY_DOWNSIDE_DOWN:
                nChairRunState = CHAIR_STATE_RUN ;
                nBackMainRunMode = BACK_MAIN_MODE_MANUAL;
                nMaunalSubMode = nMaunalSubMode_DOWN;
                break;
            case H10_KEY_DOWNSIDE_DOWN_STOP:
                nChairRunState = CHAIR_STATE_RUN ;
                nBackMainRunMode = BACK_MAIN_MODE_MANUAL;
                nMaunalSubMode = nMaunalSubMode_DOWN;
                break;
            case H10_KEY_UPSIDE_UP:
                nChairRunState = CHAIR_STATE_RUN ;
                nBackMainRunMode = BACK_MAIN_MODE_MANUAL;
                nMaunalSubMode = nMaunalSubMode_UP;
                break;
            case H10_KEY_UPSIDE_DOWN:
                nChairRunState = CHAIR_STATE_RUN ;
                nBackMainRunMode = BACK_MAIN_MODE_MANUAL;
                nMaunalSubMode = nMaunalSubMode_UP;
                break;
            case H10_KEY_UPSIDE_STOP:
                nChairRunState = CHAIR_STATE_RUN ;
                nBackMainRunMode = BACK_MAIN_MODE_MANUAL;
                nMaunalSubMode = nMaunalSubMode_UP;
                break;
            case H10_KEY_HEAT_OFF:
                nChairRunState = CHAIR_STATE_RUN ;
                bKeyBackPadHot = False;
                break;
            case H10_KEY_HEAT_ON:
                nChairRunState = CHAIR_STATE_RUN ;
                bKeyBackPadHot = True;
                break;
            case H10_KEY_BACKPAD_UP:
                nChairRunState = CHAIR_STATE_RUN ;
                bKeyBackPadUp = True;
                bKeyBackPadDown = False;
                break;
            case H10_KEY_BACKPAD_DOWN:
                nChairRunState = CHAIR_STATE_RUN ;
                bKeyBackPadUp = False;
                bKeyBackPadDown = True;
                break;
            case H10_KEY_BACKPAD_STOP:
                nChairRunState = CHAIR_STATE_RUN ;
                bKeyBackPadUp = False;
                bKeyBackPadDown = False;
                break;
            case H10_KEY_LEG_VIBRATE_ON:
                nChairRunState = CHAIR_STATE_RUN ;
                bKeyLegVibrate = True;
                break;
            case H10_KEY_LEG_VIBRATE_OFF:
                nChairRunState = CHAIR_STATE_RUN ;
                bKeyLegVibrate = False;
                break;
            default:
                break;
        }
    }
}

/*******************************************************************************
*       Main Function
*******************************************************************************/
void main (void)
{
    Sys_Init();
    Data_Init();
    nChairRunState = CHAIR_STATE_SETTLE;//开机之后先复位
    while(1)
    {
        set_WDCLR;
        Comm();
        switch(nChairRunState)
          {
          case CHAIR_STATE_SETTLE:
               Main_Settle();
               break;  
          case CHAIR_STATE_SLEEP:
               Main_Sleep_Process();
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
