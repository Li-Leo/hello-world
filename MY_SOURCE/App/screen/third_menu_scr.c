/************************************************
* COPYRIGHT 2017 Microport.
*
* NAME: third_menu_scr.c
*
* DESCRIPTION:
*
*
* REVISION HISTORY:
*   Rev 1.0 2017-03-09 xqzhao
* Initial revision.
*
* END OF FILE HEADER
************************************************/
#include "third_menu_scr.h"
#include "common.h"
#include "CtlKey.h"
#include "CtlDisplay.h"
#include "string_data.h"
#include "menu_scr.h"
#include "CtlData.h"
#include "AppUiHandle.h"
#include "Set_deli_power_scr.h"
#include "CtlData.h"
/************************************************
* Declaration
************************************************/
extern T_U8 g_last_user_set_deli_power;
/************************************************
* Variable 
************************************************/
static const TS_MENU_ITEM g_third_menu_items[E_THIRD_MENU_MAX] = {
	{STR_DAYTOTAL_RECORD},
	{STR_MEAL_BOLUS_REC},
	{STR_INFU_STOP_RECORD},
	{NULL},
};

/************************************************
* Function 
************************************************/

static TE_BOOLEAN ThirdMenuScrOnPressPlusKey()
{
	switch (MenuScrCurrSelectItem()){
		case E_THIRD_MENU_TOTAL_REC:
		    AppTotalRecMenuScreenOnPressPlusKey();
			break;
		case E_THIRD_MENU_UNINFU_REC:
			AppStopInfuRecMenuScreenOnPressPlusKey();	
			break;
		case E_THIRD_MENU_MEAL_BOLUS_REC:
		    AppInfuRecMenuScreenOnPressPlusKey();
			break;			
	}
    return E_TRUE;
}
static TE_BOOLEAN ThirdMenuScrOnPressMinusKey()
{
	switch (MenuScrCurrSelectItem()){
		case E_THIRD_MENU_TOTAL_REC:
		    AppTotalRecMenuScreenOnPressMinusKey();		
			break;
		case E_THIRD_MENU_UNINFU_REC:
			AppStopInfuRecMenuScreenOnPressMinusKey();
			break;
		case E_THIRD_MENU_MEAL_BOLUS_REC:
		    AppInfuRecMenuScreenOnPressMinusKey();
			break;			
	}	
    return E_TRUE;
}

static void ThirdMenuScrOnItemSelect(T_SINT item)
{
	switch (item){
		case E_THIRD_MENU_DELI_POWER_SET:
			break;
	}
}

static void ThirdMenuScrOnItemActive(T_SINT item)
{
	switch (item){
		case E_THIRD_MENU_TOTAL_REC:
		case E_THIRD_MENU_MEAL_BOLUS_REC:
		case E_THIRD_MENU_UNINFU_REC:
			CtlKeyEventBindHandle(E_KEY_PLUS, E_KEY_EVENT_PRESSED, ThirdMenuScrOnPressPlusKey);
			CtlKeyEventBindHandle(E_KEY_MINUS, E_KEY_EVENT_PRESSED, ThirdMenuScrOnPressMinusKey);
			break;			
		case E_THIRD_MENU_DELI_POWER_SET:
		    SetDeliPowerScrOnActive();
			break;
	}
}

static void ThirdMenuScrOnItemDraw(T_SINT item)
{
	switch (item){
		case E_THIRD_MENU_DELI_POWER_SET:
		    SetDeliPowerScrOnDraw();
			break;
	}
}

static void ThirdMenuScrOnSelectItemOK(T_SINT item)
{
	switch (item){
		case E_THIRD_MENU_TOTAL_REC:
			//DisplayUI(E_SCREEN_VIEW_TOTAL_REC);
			break;
		case E_THIRD_MENU_MEAL_BOLUS_REC:
			break;
		case E_THIRD_MENU_UNINFU_REC:
			//DisplayUI(E_SCREEN_VIEW_UNINFU_REC);
			break;	
	}
}

TE_BOOLEAN ThirdMenuScrOnHeldMainKey()
{
	DisplayUI(E_SCREEN_HOME);
    return E_TRUE;
}

/***********************************************
* Description:
*   handle screen msg
* Argument:
*
* Return:
*
************************************************/
void ThirdMenuScrOnMsg(TE_SCREEN_MESSGAE msg, T_SINT param)
{
	switch(msg)
	{
		case E_SCR_MSG_CREATE:
			MenuScrInit(E_SCREEN_THIRD_MENU, g_third_menu_items, M_NumOfArr(g_third_menu_items), 
				0);
			break;
		case E_SCR_MSG_ACTIVE:
			CtlKeyEventBindHandle(E_KEY_MAIN, E_KEY_EVENT_HELD_1S, ThirdMenuScrOnHeldMainKey);
			break;			
		case E_SCR_MSG_MENU_ITEM_SELECT:
			ThirdMenuScrOnItemSelect(param);
			break;			
		case E_SCR_MSG_MENU_ITEM_ACTIVE:
			CtlKeyEventBindHandle(E_KEY_MAIN, E_KEY_EVENT_HELD_1S, ThirdMenuScrOnHeldMainKey);
			ThirdMenuScrOnItemActive(param);
			break;			
		case E_SCR_MSG_MENU_ITEM_DRAW:
			ThirdMenuScrOnItemDraw(param);
			break;
		case E_SCR_MSG_MENU_ITEM_SELECT_OK:
			ThirdMenuScrOnSelectItemOK(param);
			break;
		case E_SCR_MSG_OK:
            //save deli power
			g_sys_para_user_set_deli_power=g_last_user_set_deli_power;
            SaveSysPara(E_SYSTEM_PARA_USER_SET_DELI_POWER,0);
            SaveRec(E_RECORD_OPERATION, E_OPERATION_USER_SET_DELI_POWER, g_sys_para_user_set_deli_power);
            UIGoBackTo(E_SCREEN_THIRD_MENU);					
			break;			
		case E_SCR_MSG_CANCEL:
			UIGoBackTo(E_SCREEN_THIRD_MENU);
			break;	
	}
	MenuScrOnMsg(msg, param);
}

//显示菜单�?
void ThirdMenuScrShowItem(TE_THIRD_MENU_ITEM item)
{
	if(GetCurrentUI() != E_SCREEN_THIRD_MENU)
	{
		UIGoBackTo(E_SCREEN_THIRD_MENU);
	}
	MenuScrSelectItem(item);
}
//显示下一个菜单项
void ThirdMenuScrShowNextItem()
{
	if(GetCurrentUI() != E_SCREEN_THIRD_MENU)
	{
		UIGoBackTo(E_SCREEN_THIRD_MENU);
	}

	MenuScrSelectNextItem();

}

