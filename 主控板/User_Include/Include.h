#include "N76E003_iar.h"
#include "SFR_Macro.h"
#include "Function_define.h"
#include "Common.h"
#include "Delay.h"
#include "IAP.h"
#include "Variable.h"
#include "Function.h"


#define T1Nuber 1384
#define LED  P15
#define RS485_EN  P05

#define Blue  P03
#define Red  P01
#define Green  P00

#define WSTART 0x21
#define START  0x22
#define DATA   0x25
#define DEND   0x26