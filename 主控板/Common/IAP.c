#include "N76E003_iar.h"
#include "SFR_Macro.h"
#include "Function_define.h"
#include "Common.h"
#include "Delay.h"
#include "IAP.h"
unsigned char TMP;
/*******************************************************************************
*       Enable IAP 
*******************************************************************************/
void Enable_IAP(void)
{
  TMP=EA;
  EA=0;
  TA=0xAA;
  TA=0x55;
  CHPCON |= 0x01;
  EA=TMP;
  return;
}
/*******************************************************************************
*       Disable IAP 
*******************************************************************************/
void Disable_IAP(void)
{
  TMP=EA;
  EA=0;
  TA=0xAA;
  TA=0x55;
  CHPCON &= 0xfe;
  EA=TMP;
  return;
}
/*******************************************************************************
*       Erase or program APROM Enable
*******************************************************************************/
void Enable_AP_Update(void)
{
  TMP=EA;
  EA=0;
  TA=0xAA;
  TA=0x55;
  IAPUEN |= 0x01; 
  EA=TMP;
  return;
}
/*******************************************************************************
*       Erase or program APROM Disable
*******************************************************************************/
void Disable_AP_Update(void)
{
  TMP=EA;
  EA=0;
  TA=0xAA;
  TA=0x55;
  IAPUEN &= 0xfe; 
  EA=TMP;
  return;
}
/*******************************************************************************
*       Erase or program CONFIG Enable
*******************************************************************************/
void Enable_CONFIG_Update(void)
{
  TMP=EA;
  EA=0;
  TA=0xAA;
  TA=0x55;
  IAPUEN |= 0x04;
  EA=TMP;
  return;
}
/*******************************************************************************
*       Erase or program CONFIG Disable
*******************************************************************************/
void Disable_CONFIG_Update(void)
{
  TMP=EA;
  EA=0;
  TA=0xAA;
  TA=0x55;
  IAPUEN &= 0xfb;
  EA=TMP;
  return;
}
/*******************************************************************************
*       IAP Trigger
*******************************************************************************/
void Trigger_IAP(void)
{
  TMP=EA;
  EA=0;
  TA=0xAA;
  TA=0x55;
  IAPTRG |= 0x01 ;
  EA=TMP;
  return;
}

/*******************************************************************************
*       APROM Page Erase
*******************************************************************************/
void Erase_AP(char page)
{
  Enable_IAP();
  Enable_AP_Update();
  IAPCN = 0x22;
  IAPFD = 0xff;
  if((page%2))
  {
    IAPAH = (page/2);
    IAPAL = 0x80;
  }
  else
  {
    IAPAH = (page/2);
    IAPAL = 0x00;
  }
  Trigger_IAP();
  Disable_AP_Update();
  Disable_IAP();
  return;
}
/*******************************************************************************
*       APROM Write
*******************************************************************************/
void Program_AP(char page,char *Byte)
{
  char ByetCount;
  Enable_IAP();
  Enable_AP_Update();
  IAPCN = 0x21;
  if((page%2))
  {
    IAPAH = (page/2);
    for(ByetCount=0x80;ByetCount<0xFF;ByetCount++)
    {
      IAPAL = ByetCount;
      IAPFD = Byte[ByetCount-0x80];
      Trigger_IAP();
    }
    IAPAL = ByetCount;
    IAPFD = Byte[ByetCount-0x80];
    Trigger_IAP();
  }
  else
  {
    IAPAH = (page/2);
    for(ByetCount=0x00;ByetCount<0x80;ByetCount++)
    {
      IAPAL = ByetCount;
      IAPFD = Byte[ByetCount];
      Trigger_IAP();
    }
  }
  Disable_AP_Update();
  Disable_IAP();
  return;
}
/*******************************************************************************
*       APROM Read
*******************************************************************************/
char Read_AP(int add)
{
  char RData;
  Enable_IAP();
  Enable_AP_Update();
  IAPCN = 0x00;
  IAPAH = add/256;
  IAPAL = add%256;
  Trigger_IAP();
  RData = IAPFD;
  Disable_AP_Update();
  Disable_IAP();
  return RData;
}
/*******************************************************************************
*       APROM Verify 
*******************************************************************************/
char Program_AP_Verify(char page,char *Byte)
{
  char ByetCount;
  char ReadData;
  Enable_IAP();
  Enable_AP_Update();
  IAPCN = 0x00;
  if((page%2))
  {
    IAPAH = (page/2);
    for(ByetCount=0x80;ByetCount<0xFF;ByetCount++)
    {
      IAPAL = ByetCount;
      Trigger_IAP();
      ReadData = IAPFD;
      if(ReadData!=Byte[ByetCount-0x80])
      {
        Disable_AP_Update();
        Disable_IAP();
        return 0x00;
      }
    }
    IAPAL = ByetCount;
    Trigger_IAP();
    ReadData = IAPFD;
    if(ReadData!=Byte[ByetCount-0x80])
    {
      Disable_AP_Update();
      Disable_IAP();
      return 0x00;
    }
  }
  else
  {
    IAPAH = (page/2);
    for(ByetCount=0x00;ByetCount<0x80;ByetCount++)
    {
      IAPAL = ByetCount;
      Trigger_IAP();
      ReadData = IAPFD;
      if(ReadData!=Byte[ByetCount])
      {
        Disable_AP_Update();
        Disable_IAP();
        return 0x00;
      }
    }
  }
  Disable_AP_Update();
  Disable_IAP();
  return 0x01;
}

/*******************************************************************************
*       CONFIG Erase
*******************************************************************************/
void Erase_CONFIG(void)
{
  Enable_IAP();
  Enable_CONFIG_Update();
  IAPCN = 0xe2;
  IAPAH = 0x00;
  IAPAL = 0x00;
  IAPFD = 0xff;
  Trigger_IAP();
  Disable_CONFIG_Update();
  Disable_IAP();
  return;
}
/*******************************************************************************
*       CONFIG Read
*******************************************************************************/
char Read_CONFIG(char CFx)
{
  char CFdata;
  Enable_IAP();
  IAPCN = 0xc0;
  IAPAH = 0x00;
  IAPAL = CFx;
  Trigger_IAP();
  CFdata = IAPFD;
  Disable_IAP();
  return CFdata;
}
/*******************************************************************************
*       CONFIG Write
*******************************************************************************/
void Program_CONFIG( char CFadd, char CFdata)
{
  Enable_IAP();
  Enable_CONFIG_Update();
  IAPCN = 0xe1;
  IAPAH = 0x00;
  IAPAL = CFadd;
  IAPFD &= CFdata;
  Trigger_IAP();
  Disable_CONFIG_Update();
  Disable_IAP();
  return;
}
/*******************************************************************************
*       EEROMWR Write
*******************************************************************************/
void EEROMWR(char add,char data)
{
  char eedata[32]={0x00};
  char eei;
  Enable_IAP();
  Enable_AP_Update();
  IAPCN = 0x00;//Read
  for(eei=0;eei<32;eei++)
  {
    IAPAH = 0x37;
    IAPAL = 0x80+eei;
    Trigger_IAP();
    eedata[eei]=IAPFD;
  }
  
  IAPCN = 0x22;//Erase
  IAPAH = 0x37;
  IAPAL = 0x80;
  IAPFD = 0xff;
  Trigger_IAP();
  
  IAPCN = 0x21;//Write
  IAPAH = 0x37;
  for(eei=0;eei<32;eei++)
  {
    if(eei==add)
    {
      IAPAL = 0x80+eei;
      IAPFD = data;
      Trigger_IAP();
    }
    else
    {
      IAPAL = 0x80+eei;
      IAPFD = eedata[eei];
      Trigger_IAP();
    }
  }
  Disable_AP_Update();
  Disable_IAP();
  return ;
}
/*******************************************************************************
*       EEROMRD Read
*******************************************************************************/
char EEROMRD(char add)
{
  char Read;
  Enable_IAP();
  Enable_AP_Update();
  IAPCN = 0x00;
  IAPAH = 0x37;
  IAPAL = add+0x80;
  Trigger_IAP();
  Read=IAPFD;
  Disable_AP_Update();
  Disable_IAP();
  return Read;
}