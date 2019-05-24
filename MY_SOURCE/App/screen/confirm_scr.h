/************************************************
* COPYRIGHT 2017 Microport.
*
* NAME: confirm_scr.h
*
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-03-08 xqzhao
* Initial revision.
*
* END OF FILE HEADER
************************************************/
#ifndef CONFIRM_SCR
#define CONFIRM_SCR

#include "AppUiScreen.h"

/************************************************
* Declaration
************************************************/
#define CONFIRM_SCR_SHOW_TEXT_MAX 20

//handle UI message
void ConfirmScrOnMsg(TE_SCREEN_MESSGAE msg, T_SINT param);

void ConfirmScrShow(TE_SCREEN_ID parent_id, T_CHAR msg[]);
//ÏÔÊ¾Ö´ÐÐÂð?
void ConfirmExecScrShow();

#endif
