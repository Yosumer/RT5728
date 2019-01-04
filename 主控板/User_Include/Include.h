#include "N76E003_iar.h"
#include "SFR_Macro.h"
#include "Function_define.h"
#include "Common.h"
#include "Delay.h"
#include "IAP.h"
#include "Variable.h"
#include "Function.h"
#include "ControlBox.h"

#define On      1
#define Off     0
#define True    1
#define False   0

#define T1Nuber     1384
#define RS485_EN    P05
#define Power       P02
#define Hot         P12
#define Vibrate     P11

#define WSTART 0x21
#define START  0x22
#define DATA   0x25
#define DEND   0x26
