/************************************************
* COPYRIGHT 2017 Microport.
*
* NAME: third_menu_scr.h
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
#ifndef THIRD_MENU_SCR
#define THIRD_MENU_SCR

#include "AppUiScreen.h"

/************************************************
* Declaration
************************************************/
typedef enum
{
	E_THIRD_MENU_TOTAL_REC, //日总量记录
	E_THIRD_MENU_MEAL_BOLUS_REC, //餐前量记录
	E_THIRD_MENU_UNINFU_REC, //停输记录
	E_THIRD_MENU_DELI_POWER_SET, //输注力调整
	E_THIRD_MENU_MAX,
}TE_THIRD_MENU_ITEM;

//handle UI message
void ThirdMenuScrOnMsg(TE_SCREEN_MESSGAE msg, T_SINT param);
//显示菜单项
void ThirdMenuScrShowItem(TE_THIRD_MENU_ITEM item);
//显示下一个菜单项
void ThirdMenuScrShowNextItem();
#endif
