/************************************************
* COPYRIGHT 2017 Microport.
*
* NAME: confirm_scr.c
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
#include "confirm_scr.h"
#include "common.h"
#include "CtlKey.h"
#include "CtlDisplay.h"
#include "string_data.h"
#include "string.h"
#include "CtlAlarm.h"

/************************************************
* Declaration
************************************************/

/************************************************
* Variable 
************************************************/
static T_CHAR g_confirm_scr_show_text[CONFIRM_SCR_SHOW_TEXT_MAX];
static TE_SCREEN_ID g_confirm_scr_parent_id;

/************************************************
* Function 
************************************************/

/***********************************************
* Description:
*   draw screen
* Argument:
*
* Return:
*
************************************************/
static void ConfirmScrOnDraw()
{
	DisplayTextAtXCenter(MIDDLE_LINE_Y, g_confirm_scr_show_text);
}

/***********************************************
* Description:
*   handle main key press
* Argument:
*
* Return:
*
************************************************/
static TE_BOOLEAN ConfirmScrOnPressMainKey(void)
{
    return E_TRUE;
}

/***********************************************
* Description:
*   handle plus key press
* Argument:
*
* Return:
*
************************************************/
static TE_BOOLEAN ConfirmScrOnPressPlusKey(void)
{
	SetPlayOnceNoVibration();
	UISendMsg(g_confirm_scr_parent_id, E_SCR_MSG_OK, 0);
    return E_TRUE;
}

/***********************************************
* Description:
*   handle minus key press
* Argument:
*
* Return:
*
************************************************/
static TE_BOOLEAN ConfirmScrOnPressMinusKey(void)
{
	UISendMsg(g_confirm_scr_parent_id, E_SCR_MSG_CANCEL, 0);
    return E_TRUE;
}

/***********************************************
* Description:
*   handle when screen create
* Argument:
*
* Return:
*
************************************************/
// static void ConfirmScrOnCreate()
// {

// }

/***********************************************
* Description:
*   handle when screen active(show)
* Argument:
*
* Return:
*
************************************************/
static void ConfirmScrOnActive()
{
	CtlKeyEventBindHandle(E_KEY_MAIN, E_KEY_EVENT_PRESSED, ConfirmScrOnPressMainKey);
	CtlKeyEventBindHandle(E_KEY_PLUS, E_KEY_EVENT_PRESSED, ConfirmScrOnPressPlusKey);
	CtlKeyEventBindHandle(E_KEY_MINUS, E_KEY_EVENT_PRESSED, ConfirmScrOnPressMinusKey);
}

/***********************************************
* Description:
*   handle when screen deactive(hide)
* Argument:
*
* Return:
*
************************************************/
// static void ConfirmScrOnDeactive()
// {

// }

/***********************************************
* Description:
*   handle screen msg
* Argument:
*
* Return:
*
************************************************/
void ConfirmScrOnMsg(TE_SCREEN_MESSGAE msg, T_SINT param)
{
	switch(msg)
	{
		case E_SCR_MSG_CREATE:
		//ConfirmScrOnCreate();
		break;
		case E_SCR_MSG_ACTIVE:
		ConfirmScrOnActive();
		break;
		case E_SCR_MSG_DRAW:
		ConfirmScrOnDraw();
		break;
		case E_SCR_MSG_DEACTIVE:
		//ConfirmScrOnDeactive();
		break;
	}
}

//ÏÔÊ¾Ö´ÐÐÂð?
void ConfirmExecScrShow()
{
	strcpy(g_confirm_scr_show_text, STR_IS_EXEC);
	g_confirm_scr_parent_id = GetCurrentUI();
	DisplayUI(E_SCREEN_CONFIRM);
    SetPlayOnceNoVibration();
}