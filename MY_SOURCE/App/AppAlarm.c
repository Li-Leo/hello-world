/************************************************
* COPYRIGHT 2016.
*
* NAME: AppAlarm.c
*
* DESCRIPTION:
*   alarm module
*
* REVISION HISTORY:
*	Created by Kevin Shi at 06/20/2016
*
* END OF FILE HEADER
************************************************/
#include "AppAlarm.h"
#include "AppuiScreen.h"
#include "..\Kernel\CtlAlarm.h"
#include "..\Kernel\CtlTimer.h"
#include "..\Kernel\CtlEvent.h"
#include "..\Kernel\CtlTime.h"
#include "..\Kernel\CtlData.h"
#include "..\Kernel\CtlKey.h"
#include "..\Kernel\CtlCommon.h"
#include "..\Driver\DrvLcd.h"
#include "..\Kernel\Fonts.h"
#include <assert.h>
#include <stdlib.h>

TS_ALARM_BITS g_alarm_bits;
TE_ALARM_LEVEL g_cur_alarm_level = E_ALARM_NONE;

/* high alarm link */
TS_DOUBLE_LINK_NODE g_high_alarm_node[HIGH_ALARM_COUNT];
TS_DOUBLE_LINK g_high_alarm_link;
/* low alarm link */
TS_DOUBLE_LINK_NODE g_low_alarm_node[LOW_ALARM_COUNT];
TS_DOUBLE_LINK g_low_alarm_link;
/* middle alarm link */
TS_DOUBLE_LINK_NODE g_middle_alarm_node[MIDDLE_ALARM_COUNT];
TS_DOUBLE_LINK g_middle_alarm_link;
/* remind alarm link */
TS_DOUBLE_LINK_NODE g_remind_alarm_node[REMIND_ALARM_COUNT];
TS_DOUBLE_LINK g_remind_alarm_link;

/* battery low and deplete time*/
T_U32 g_monitor_battery_low_time;
T_U32 g_monitor_battery_deplete_time;
TE_BOOLEAN g_flag_battery_low = E_FALSE;

extern TE_SCREEN_ID g_current_ui;
extern T_U16 g_alarm_status;

/***********************************************
* Description:
*   alarm module initialization,g_alarm_bits will be initialized here.
* Argument:
*
* Return:
*
************************************************/
void AlarmInit(void)
{        
    /* init alarm bits information */
    g_alarm_bits.alarm_remind_bits.alarm_bits = 0;
    g_alarm_bits.alarm_low_bits.alarm_bits = 0;
    g_alarm_bits.alarm_middle_bits.alarm_bits = 0;
    g_alarm_bits.alarm_high_bits.alarm_bits = 0;

    DoubleLinkInit(&g_high_alarm_link,HIGH_ALARM_COUNT,g_high_alarm_node);
    DoubleLinkInit(&g_low_alarm_link,LOW_ALARM_COUNT,g_low_alarm_node);
    DoubleLinkInit(&g_middle_alarm_link,MIDDLE_ALARM_COUNT,g_middle_alarm_node);
    DoubleLinkInit(&g_remind_alarm_link,REMIND_ALARM_COUNT,g_remind_alarm_node);
    
}


/***********************************************
* Description:
*   Set corresponding alarms' bits.       
* Argument:
*   e_alarm_id: alarm id ,type is TE_ALARM
*
* Return:
*
************************************************/
void AlarmSet(TE_ALARM_ID e_alarm_id)
{
    TE_ALARM_LEVEL e_alarm_level;
    T_U16 bit_position;
    
    M_Assert(e_alarm_id < E_ALARM_COUNT);

    /* already set ,do nothing */
    if (E_TRUE == AlarmIsSet(e_alarm_id))
	{
        return;
    }
    M_CommonPrint("alarm[%d] set\n", e_alarm_id);
    e_alarm_level = AlarmGetLevel(e_alarm_id);    

    /* remind level alarm id */
    if (E_ALARM_REMIND == e_alarm_level)
	{
        bit_position = (T_U16)e_alarm_id;
        g_alarm_bits.alarm_remind_bits.alarm_bits |= (T_U16)(1<<bit_position);

        /* operation in double link */
        DoubleLinkDel(&g_remind_alarm_link,(T_U32)e_alarm_id);
        DoubleLinkInsert(&g_remind_alarm_link,(T_U32)e_alarm_id);
    }
    /* low level alarm id */
    else if (E_ALARM_LOW == e_alarm_level)
	{

		bit_position = (T_U16)e_alarm_id - REMIND_ALARM_COUNT;
        g_alarm_bits.alarm_low_bits.alarm_bits |= (T_U16)(1<<bit_position);

        /* operation in double link */
        DoubleLinkDel(&g_low_alarm_link,(T_U32)e_alarm_id);
        DoubleLinkInsert(&g_low_alarm_link,(T_U32)e_alarm_id);
    }
    /* middle level alarm id */
    else if (E_ALARM_MIDDLE == e_alarm_level)
	{
        bit_position = (T_U16)e_alarm_id - (REMIND_ALARM_COUNT + LOW_ALARM_COUNT);
        g_alarm_bits.alarm_middle_bits.alarm_bits |= (T_U16)(1<<bit_position);

        /* operation in double link */
        DoubleLinkDel(&g_middle_alarm_link,(T_U32)e_alarm_id);
        DoubleLinkInsert(&g_middle_alarm_link,(T_U32)e_alarm_id);
    }
    /* high level alarm id */
    else if (E_ALARM_HIGH == e_alarm_level)
	{
        bit_position = (T_U16)e_alarm_id - (REMIND_ALARM_COUNT+LOW_ALARM_COUNT+MIDDLE_ALARM_COUNT);
        g_alarm_bits.alarm_high_bits.alarm_bits |= (T_U16)(1<<bit_position);

        /* operation in double link */
        DoubleLinkDel(&g_high_alarm_link,(T_U32)e_alarm_id);
        DoubleLinkInsert(&g_high_alarm_link,(T_U32)e_alarm_id);
    }

    /* send event E_EVENT_ALARM_NOTIFY */
    CtlEventPost(E_EVENT_ALARM_NOTIFY,(T_U32)(e_alarm_id));
    
}


/***********************************************
* Description:
*   Clear corresponding alarms' bits.
* Argument:
*   e_alarm_id:
*
* Return:
*
************************************************/
void AlarmClear(TE_ALARM_ID e_alarm_id)
{
    TE_ALARM_LEVEL e_alarm_level;
    T_U16 bit_position;
   
    M_Assert(e_alarm_id < E_ALARM_COUNT);    

    /* this alarm have not been set, do nothing. */
    if (E_FALSE == AlarmIsSet(e_alarm_id))
	{
        return;
    }

    M_CommonPrint("alarm[%d] clear\n", e_alarm_id);
    e_alarm_level = AlarmGetLevel(e_alarm_id);

    if (E_ALARM_REMIND == e_alarm_level)
	{
        bit_position = (T_U16)e_alarm_id;
        g_alarm_bits.alarm_remind_bits.alarm_bits &= (T_U16)(~(1<<bit_position));

        /* operation in double link */
        DoubleLinkDel(&g_remind_alarm_link,(T_U32)e_alarm_id);        
    }
    else if (E_ALARM_LOW == e_alarm_level)
	{
        bit_position = (T_U16)e_alarm_id - REMIND_ALARM_COUNT;
        g_alarm_bits.alarm_low_bits.alarm_bits &= (T_U16)(~(1<<bit_position));

        /* operation in double link */
        DoubleLinkDel(&g_low_alarm_link,(T_U32)e_alarm_id);        
    }
    else if (E_ALARM_MIDDLE == e_alarm_level)
	{
        bit_position = (T_U16)e_alarm_id - (REMIND_ALARM_COUNT + LOW_ALARM_COUNT);
        g_alarm_bits.alarm_middle_bits.alarm_bits &= (T_U16)(~(1<<bit_position));
		
        /* operation in double link */
        DoubleLinkDel(&g_middle_alarm_link,(T_U32)e_alarm_id);
    }
    else if (E_ALARM_HIGH == e_alarm_level)
	{
        bit_position = (T_U16)e_alarm_id - (REMIND_ALARM_COUNT + LOW_ALARM_COUNT + MIDDLE_ALARM_COUNT);
        g_alarm_bits.alarm_high_bits.alarm_bits &= (T_U16)(~(1<<bit_position));

        /* operation in double link */
        DoubleLinkDel(&g_high_alarm_link,(T_U32)e_alarm_id);
    }
     
    /* send event E_EVENT_ALARM_CLEAR */
    CtlEventPost(E_EVENT_ALARM_CLEAR,(T_U32)(e_alarm_id));
    
}


/***********************************************
* Description:
*   Clear all alarm bits.
* Argument:
*
* Return:
*
************************************************/
void AlarmClearAll(void)
{
    g_alarm_bits.alarm_remind_bits.alarm_bits = 0;
    g_alarm_bits.alarm_low_bits.alarm_bits = 0;
    g_alarm_bits.alarm_middle_bits.alarm_bits = 0;
    g_alarm_bits.alarm_high_bits.alarm_bits = 0;

    /* operation on link */
    DoubleLinkInit(&g_high_alarm_link,HIGH_ALARM_COUNT,g_high_alarm_node);
    DoubleLinkInit(&g_low_alarm_link,LOW_ALARM_COUNT,g_low_alarm_node);
    DoubleLinkInit(&g_middle_alarm_link,MIDDLE_ALARM_COUNT,g_middle_alarm_node);
    DoubleLinkInit(&g_remind_alarm_link,E_ALARM_REMIND,g_remind_alarm_node);
}


/***********************************************
* Description:
*   Are corresponding alamrs set?
* Argument:
*   e_alarm_id: alarm id , type is TE_ALARM_ID
*
* Return:
*   if correspond alarm has been set ,will return E_TRUE
************************************************/
TE_BOOLEAN AlarmIsSet(TE_ALARM_ID e_alarm_id)
{
    TE_ALARM_LEVEL e_alarm_level;
    T_U16 bit_position;
    
    M_Assert(e_alarm_id < E_ALARM_COUNT);

    e_alarm_level = AlarmGetLevel(e_alarm_id);

    if (E_ALARM_REMIND == e_alarm_level)
	{
        bit_position = (T_U16)e_alarm_id;
        if (g_alarm_bits.alarm_remind_bits.alarm_bits & (T_U16)(1<<bit_position))
		{
            return E_TRUE;
        }
    }
    else if (E_ALARM_LOW == e_alarm_level)
	{
        bit_position = (T_U16)e_alarm_id - REMIND_ALARM_COUNT;
        if (g_alarm_bits.alarm_low_bits.alarm_bits & (T_U16)(1<<bit_position))
		{
            return E_TRUE;
        }
    }
    else if (E_ALARM_MIDDLE == e_alarm_level)
	{
        bit_position = (T_U16)e_alarm_id - (REMIND_ALARM_COUNT + LOW_ALARM_COUNT);
        if (g_alarm_bits.alarm_middle_bits.alarm_bits & (T_U16)(1<<bit_position))
		{
            return E_TRUE;
        }
    }
    else if (E_ALARM_HIGH == e_alarm_level)
	{
        bit_position = (T_U16)e_alarm_id - (REMIND_ALARM_COUNT + LOW_ALARM_COUNT + MIDDLE_ALARM_COUNT);
        if (g_alarm_bits.alarm_high_bits.alarm_bits & (T_U16)(1<<bit_position))
		{
            return E_TRUE;
        }
    }
    return E_FALSE;    

}


TE_BOOLEAN AlarmHaveHighError(void)
{
    if (g_alarm_bits.alarm_high_bits.alarm_bits)
	{
        return E_TRUE; 
    }    
    return E_FALSE;
}

TE_BOOLEAN AlarmHaveMiddleError(void)
{
    if (g_alarm_bits.alarm_middle_bits.alarm_bits)
	{
        return E_TRUE; 
    }    
    return E_FALSE;
}

TE_BOOLEAN AlarmHaveLowError(void)
{
    if (g_alarm_bits.alarm_low_bits.alarm_bits)
	{
        return E_TRUE; 
    }    
    return E_FALSE;
}

TE_BOOLEAN AlarmHaveRemind(void)
{
    if (g_alarm_bits.alarm_remind_bits.alarm_bits)
	{
        return E_TRUE; 
    }    
    return E_FALSE;
}


/***********************************************
* Description:
*    Get highest alarm level.
* Argument:
*
* Return:
*   return highest alarm level.
************************************************/
TE_ALARM_LEVEL AlarmGetHighestLevel(void)
{
    if (g_alarm_bits.alarm_high_bits.alarm_bits)
	{
        return E_ALARM_HIGH;
    }
    else if (g_alarm_bits.alarm_middle_bits.alarm_bits)
	{
        return E_ALARM_MIDDLE;
    }
    else if (g_alarm_bits.alarm_low_bits.alarm_bits)
	{
        return E_ALARM_LOW;
    }
    else if (g_alarm_bits.alarm_remind_bits.alarm_bits)
	{
        return E_ALARM_REMIND;
    }
    return E_ALARM_NONE;
}


/***********************************************
* Description:
*   one alarm's alarm level
* Argument:
*   e_alarm_id: alarm id ,type is TE_ALARM_ID
*   
* Return:
*   return one alarm's alarm level
************************************************/
TE_ALARM_LEVEL AlarmGetLevel(TE_ALARM_ID e_alarm_id)
{
    M_Assert(e_alarm_id < E_ALARM_COUNT);

    if (e_alarm_id < REMIND_ALARM_COUNT)
	{
        return E_ALARM_REMIND;
    }
    else if (e_alarm_id < (REMIND_ALARM_COUNT + LOW_ALARM_COUNT))
	{
        return E_ALARM_LOW;
    }
    else if (e_alarm_id < (REMIND_ALARM_COUNT +LOW_ALARM_COUNT + MIDDLE_ALARM_COUNT))
	{
        return E_ALARM_MIDDLE;
    }
    else if (e_alarm_id < E_ALARM_COUNT)
	{
        return E_ALARM_HIGH;
    }
    return E_ALARM_NONE;
}


/***********************************************
* Description:
*   indicate current alarm level is changed or not.
* Argument:
*
* Return:
*   E_TRUE,indicate alarm level has changed;E_FALSE,means has not changed.
************************************************/
TE_BOOLEAN AlarmIsLevelChange(void)
{
    TE_ALARM_LEVEL e_alarm_level;

    e_alarm_level = AlarmGetHighestLevel();
    
    if (g_cur_alarm_level != e_alarm_level)
	{
        g_cur_alarm_level = e_alarm_level;
      
        return E_TRUE;
    }
    return E_FALSE;
}


/***********************************************
* Description:
*   return the first alarm id in highest priority for alarm monitor to show
* Argument:
*
* Return:
*   alarm id,the type is TE_ALARM_ID
************************************************/
TE_ALARM_ID AlarmShowFirstOfHighestPriority(void)
{
    TE_ALARM_ID e_alarm_id;
    TE_ALARM_LEVEL e_alarm_level;

    e_alarm_level = AlarmGetHighestLevel();

    if (E_ALARM_HIGH == e_alarm_level)
	{
        e_alarm_id = (TE_ALARM_ID)DoubleLinkFirstData(&g_high_alarm_link);
        return e_alarm_id;
    }
    else if (E_ALARM_LOW == e_alarm_level)
	{
        e_alarm_id = (TE_ALARM_ID)DoubleLinkFirstData(&g_low_alarm_link);
        return e_alarm_id;
    }
    else if (E_ALARM_MIDDLE == e_alarm_level)
	{
        e_alarm_id = (TE_ALARM_ID)DoubleLinkFirstData(&g_middle_alarm_link);
        return e_alarm_id;
    }
    else if (E_ALARM_REMIND == e_alarm_level)
	{
        e_alarm_id = (TE_ALARM_ID)DoubleLinkFirstData(&g_remind_alarm_link);
        return e_alarm_id;
    }
    
    return E_ALARM_COUNT;    
}

void AlarmPlay(TE_ALARM_TYPE type)
{
    AlarmPlayEx(type, E_TRUE);
}

/***********************************************
* Description:  play alarm beep and VibMotor.
*
* Argument:
*
* Return:
*
************************************************/
void AlarmPlayEx(TE_ALARM_TYPE type, TE_BOOLEAN is_play_at_once)
{
	M_Assert(type < E_ALARM_TYPE_MAX);
	switch(type)
	{
		case E_ALARM_BEEP_ONE_WITH_VIBR:
    		CtlTimerBindLowPowerHandle(E_TIMER_ALARM_PLAY_INTERVAL,SetPlayOnce);
            if(is_play_at_once)SetPlayOnce();
			break;
		case E_ALARM_BEEP_ONE_NO_VIBR:
    		CtlTimerBindLowPowerHandle(E_TIMER_ALARM_PLAY_INTERVAL,SetPlayOnceNoVibration);
			if(is_play_at_once)SetPlayOnceNoVibration();
			break;
		case E_ALARM_BEEP_TWO_WITH_VIBR:
    		CtlTimerBindLowPowerHandle(E_TIMER_ALARM_PLAY_INTERVAL,SetPlayTwice);
			if(is_play_at_once)SetPlayTwice();
			break;
        case E_ALARM_BEEP_TWO_WITHOUT_VIBR:
            CtlTimerBindLowPowerHandle(E_TIMER_ALARM_PLAY_INTERVAL,SetPlayTwiceWithoutVib);
			if(is_play_at_once)SetPlayTwiceWithoutVib();
			break;
		default:
    		//CtlTimerBindHandle(E_TIMER_ALARM_PLAY_INTERVAL,SetPlayNone);
			break;
	}
	CtlTimerSetRepeat(E_TIMER_ALARM_PLAY_INTERVAL, ALARM_PLAY_INTERVAL);
}

/***********************************************
* Description: show alarm message .
*
* Argument:
*
* Return:
*
************************************************/
/*void AlarmShow(TE_ALARM_ID e_alarm_id)
{
	M_Assert(e_alarm_id <= E_ALARM_COUNT);
	T_U8* buffer = 0;
	buffer = (T_U8 *)malloc(256 * sizeof(T_U8));
	T_U8 grid_x = 0;
	TE_STRING_ID string_id;
	//DrawTextAtLineTwoWithFlash(grid_x, buffer, E_DRAW_LCD_TYPE_ICON_ARRAY);
	switch(e_alarm_id)
	{
		case E_ALARM_REMIND_DOSE_LEFT_LESS_THAN_INFU_VOL_ID:
			string_id = alarm_msg_table[0];
			GetStringBuffer(buffer, string_id);
			DrawTextAtLineTwo(grid_x, buffer, E_DRAW_LCD_TYPE_ASCII_CHAR);
			CtlTimerBindHandle(E_TIMER_WAITING_MCU_RESET, AppTimerHandleClearRemindInfo);
			CtlTimerSet(E_TIMER_ALARM_SHOW_KEEP, ALARM_SHOW_KEEP);
			break;
		case E_ALARM_REMIND_DAY_TOTAL_OVERRUN_ID:
			string_id = alarm_msg_table[1];
			GetStringBuffer(buffer, string_id);
			DrawTextAtLineTwo(grid_x, buffer, E_DRAW_LCD_TYPE_ASCII_CHAR);
			CtlTimerBindHandle(E_TIMER_WAITING_MCU_RESET, AppTimerHandleClearRemindInfo);
			CtlTimerSet(E_TIMER_ALARM_SHOW_KEEP, ALARM_SHOW_KEEP);
			break;
		case E_ALARM_REMIND_DAY_TOTAL_OUT_OF_RANGE_ID:
			string_id = alarm_msg_table[2];
			GetStringBuffer(buffer, string_id);
			DrawTextAtLineTwo(grid_x, buffer, E_DRAW_LCD_TYPE_ASCII_CHAR);
			CtlTimerBindHandle(E_TIMER_WAITING_MCU_RESET, AppTimerHandleClearRemindInfo);
			CtlTimerSet(E_TIMER_ALARM_SHOW_KEEP, ALARM_SHOW_KEEP);
			break;
		case E_ALARM_LOW_BATTERY_ID:
			string_id = alarm_msg_table[3];
			GetStringBuffer(buffer, string_id);
			grid_x = 0;
			DrawTextAtLineTwo(grid_x, buffer, E_DRAW_LCD_TYPE_ASCII_CHAR);
			break;
		case E_ALARM_DOSE_NEAR_EMPTY_ID:
			break;
		case E_ALARM_BATTERY_EXHAUSTION_ID:
			string_id = alarm_msg_table[5];
			GetStringBuffer(buffer, string_id);
			DrawTextAtLineTwo(grid_x, buffer, E_DRAW_LCD_TYPE_ASCII_CHAR);
			break;
		case E_ALARM_DOSE_EMPTY_ID:
			string_id = alarm_msg_table[6];
			GetStringBuffer(buffer, string_id);
			DrawTextAtLineTwo(grid_x, buffer, E_DRAW_LCD_TYPE_ASCII_CHAR);
			break;
		case E_ALARM_OCCLUSION_ID:
			string_id = alarm_msg_table[7];
			GetStringBuffer(buffer, string_id);
			DrawTextAtLineTwo(grid_x, buffer, E_DRAW_LCD_TYPE_ASCII_CHAR);
			break;
		case E_ALARM_DELI_SWITCH_OFF_ID:
			string_id = alarm_msg_table[8];
			GetStringBuffer(buffer, string_id);
			DrawTextAtLineTwo(grid_x, buffer, E_DRAW_LCD_TYPE_ASCII_CHAR);
			break;
		case E_ALARM_KEY_STUCK_ID:
			string_id = alarm_msg_table[9];
			GetStringBuffer(buffer, string_id);
			DrawTextAtLineTwo(grid_x, buffer, E_DRAW_LCD_TYPE_ASCII_CHAR);
			break;
		case E_ALARM_ENCODER_ERROR_ID:
			string_id = alarm_msg_table[10];
			GetStringBuffer(buffer, string_id);
			DrawTextAtLineTwo(grid_x, buffer, E_DRAW_LCD_TYPE_ASCII_CHAR);
			break;
		case E_ALARM_HALL_ERROR_ID:
			string_id = alarm_msg_table[11];
			GetStringBuffer(buffer, string_id);
			DrawTextAtLineTwo(grid_x, buffer, E_DRAW_LCD_TYPE_ASCII_CHAR);
			break;
		case E_ALARM_MCU_ERROR_ID:
			string_id = alarm_msg_table[12];
			GetStringBuffer(buffer, string_id);
			DrawTextAtLineTwo(grid_x, buffer, E_DRAW_LCD_TYPE_ASCII_CHAR);
			break;
		case E_ALARM_FRAM_ERROR_ID:
			string_id = alarm_msg_table[13];
			GetStringBuffer(buffer, string_id);
			DrawTextAtLineTwo(grid_x, buffer, E_DRAW_LCD_TYPE_ASCII_CHAR);
			break;
		case E_ALARM_MOTOR_ERROR_ID:
			string_id = alarm_msg_table[14];
			GetStringBuffer(buffer, string_id);
			DrawTextAtLineTwo(grid_x, buffer, E_DRAW_LCD_TYPE_ASCII_CHAR);
			break;
		case E_ALARM_ADC_ERROR_ID:
			string_id = alarm_msg_table[15];
			GetStringBuffer(buffer, string_id);
			DrawTextAtLineTwo(grid_x, buffer, E_DRAW_LCD_TYPE_ASCII_CHAR);
			break;
		case E_ALARM_SPI_COMM_FAIL_ID:
			string_id = alarm_msg_table[16];
			GetStringBuffer(buffer, string_id);
			DrawTextAtLineTwo(grid_x, buffer, E_DRAW_LCD_TYPE_ASCII_CHAR);
			break;
		default:
			break; 
	}
    //CtlTimerSet(E_TIMER_ALARM_SHOW_KEEP, ALARM_SHOW_KEEP);
	free(buffer);
	buffer = 0;
}*/

void AlarmShow(TE_ALARM_ID e_alarm_id)
{
	M_Assert(e_alarm_id <= E_ALARM_COUNT);
	//T_U8 grid_x = 0;

	switch(e_alarm_id)
	{
//			case E_ALARM_REMIND_DOSE_LEFT_LESS_THAN_INFU_VOL_ID:
//				g_alarm_status = (T_U16)e_alarm_id;
//				DisplayUI(E_SCREEN_SHOW_REMIND_INFO);
//	            /*CtlTimerClearAllOfScreen();
//	        	ClearBottomLine();
//				DrawTextAtLineTwo(grid_x, &gg_song_typeface_16[178+32*0x64], E_DRAW_LCD_TYPE_CHINESE_WORD);
//				DrawTextAtLineTwo(grid_x+16, &gg_song_typeface_16[178+32*0x6f], E_DRAW_LCD_TYPE_CHINESE_WORD);
//				DrawTextAtLineTwo(grid_x+32, &gg_song_typeface_16[178+32*0x6c], E_DRAW_LCD_TYPE_CHINESE_WORD);
//				DrawTextAtLineTwo(grid_x+48, &gg_song_typeface_16[178+32*0x24], E_DRAW_LCD_TYPE_CHINESE_WORD);
//				DrawTextAtLineTwo(grid_x+64, &gg_song_typeface_16[178+32*0x1f], E_DRAW_LCD_TYPE_CHINESE_WORD);
//				DrawTextAtLineTwo(grid_x+80, &gg_song_typeface_16[178+32*0x4f], E_DRAW_LCD_TYPE_CHINESE_WORD);
//				DrawTextAtLineTwo(grid_x+96, &gg_song_typeface_16[178+32*0x38], E_DRAW_LCD_TYPE_CHINESE_WORD);
//				DrawTextAtLineTwo(grid_x+112, &gg_song_typeface_16[178+32*0x46], E_DRAW_LCD_TYPE_CHINESE_WORD);
//				CtlTimerBindHandle(E_TIMER_ALARM_SHOW_KEEP, AppTimerHandleClearRemindInfo);
//				CtlTimerSet(E_TIMER_ALARM_SHOW_KEEP, ALARM_SHOW_KEEP);
//				AlarmClear(E_ALARM_REMIND_DOSE_LEFT_LESS_THAN_INFU_VOL_ID);*/
//				break;
//			case E_ALARM_REMIND_DAY_TOTAL_OVERRUN_ID:
//				g_alarm_status = (T_U16)e_alarm_id;
//				DisplayUI(E_SCREEN_SHOW_REMIND_INFO);
//	            /*CtlTimerClearAllOfScreen();
//	        	ClearBottomLine();
//				DrawTextAtLineTwo(grid_x, &gg_song_typeface_16[178+32*0x30], E_DRAW_LCD_TYPE_CHINESE_WORD);
//				DrawTextAtLineTwo(grid_x+16, &gg_song_typeface_16[178+32*0x48], E_DRAW_LCD_TYPE_CHINESE_WORD);
//				DrawTextAtLineTwo(grid_x+32, &gg_song_typeface_16[178+32*0x24], E_DRAW_LCD_TYPE_CHINESE_WORD);
//				DrawTextAtLineTwo(grid_x+48, &gg_song_typeface_16[178+32*0x77], E_DRAW_LCD_TYPE_CHINESE_WORD);
//				DrawTextAtLineTwo(grid_x+64, &gg_song_typeface_16[178+32*0x78], E_DRAW_LCD_TYPE_CHINESE_WORD);
//				DrawTextAtLineTwo(grid_x+80, &gg_song_typeface_16[178+32*0x79], E_DRAW_LCD_TYPE_CHINESE_WORD);
//				CtlTimerBindHandle(E_TIMER_ALARM_SHOW_KEEP, AppTimerHandleClearRemindInfo);
//				CtlTimerSet(E_TIMER_ALARM_SHOW_KEEP, ALARM_SHOW_KEEP);
//				AlarmClear(E_ALARM_REMIND_DAY_TOTAL_OVERRUN_ID);*/
//				break;
//			case E_ALARM_REMIND_DAY_TOTAL_OUT_OF_RANGE_ID:
//				g_alarm_status = (T_U16)e_alarm_id;
//				DisplayUI(E_SCREEN_SHOW_REMIND_INFO);
//	            /*CtlTimerClearAllOfScreen();
//	        	ClearBottomLine();
//				DrawTextAtLineTwo(grid_x, &gg_song_typeface_16[178+32*0x30], E_DRAW_LCD_TYPE_CHINESE_WORD);
//				DrawTextAtLineTwo(grid_x+16, &gg_song_typeface_16[178+32*0x48], E_DRAW_LCD_TYPE_CHINESE_WORD);
//				DrawTextAtLineTwo(grid_x+32, &gg_song_typeface_16[178+32*0x24], E_DRAW_LCD_TYPE_CHINESE_WORD);
//				DrawTextAtLineTwo(grid_x+48, &gg_song_typeface_16[178+32*0x77], E_DRAW_LCD_TYPE_CHINESE_WORD);
//				DrawTextAtLineTwo(grid_x+64, &gg_song_typeface_16[178+32*0x78], E_DRAW_LCD_TYPE_CHINESE_WORD);
//				DrawTextAtLineTwo(grid_x+90, &gg_song_typeface_16[178+32*0x7a], E_DRAW_LCD_TYPE_CHINESE_WORD);
//				DrawTextAtLineTwo(grid_x+96, &gg_song_typeface_16[178+32*0x7b], E_DRAW_LCD_TYPE_CHINESE_WORD);
//				CtlTimerBindHandle(E_TIMER_ALARM_SHOW_KEEP, AppTimerHandleClearRemindInfo);
//				CtlTimerSet(E_TIMER_ALARM_SHOW_KEEP, ALARM_SHOW_KEEP);
//				AlarmClear(E_ALARM_REMIND_DAY_TOTAL_OUT_OF_RANGE_ID);*/
//				break;
		case E_ALARM_KEY_STUCK_ID:
			//CtlKeyClearAllHandle();
			/*if(g_current_ui == E_SCREEN_HOME)
			{
            	ClearBottomLine();
				DrawTextAtLineTwo(grid_x,&gg_song_typeface_16[178+32*0x68], E_DRAW_LCD_TYPE_CHINESE_WORD);
				DrawTextAtLineTwo(grid_x+16,&gg_song_typeface_16[178+32*0x66], E_DRAW_LCD_TYPE_CHINESE_WORD);
				DrawTextAtLineTwo(grid_x+32,&gg_song_typeface_16[178+32*0x58], E_DRAW_LCD_TYPE_CHINESE_WORD);
				DrawTextAtLineTwo(grid_x+48,&gg_song_typeface_16[178+32*0x71], E_DRAW_LCD_TYPE_CHINESE_WORD);
				DrawTextAtLineTwo(grid_x+64,&gg_song_typeface_16[178+32*0x8b+16*0x0a], E_DRAW_LCD_TYPE_ASCII_CHAR);
				DrawTextAtLineTwo(grid_x+72,&gg_song_typeface_16[178+32*0x8b+16*0x00], E_DRAW_LCD_TYPE_ASCII_CHAR);
				DrawTextAtLineTwo(grid_x+80,&gg_song_typeface_16[178+32*0x8b+16*0x01], E_DRAW_LCD_TYPE_ASCII_CHAR);
			}*/
			g_alarm_status = (T_U16)e_alarm_id;
			DisplayUI(E_SCREEN_SHOW_SYSTEM_ERROR);
			break;
		case E_ALARM_MOTOR_ERROR_ID:
			//CtlKeyClearAllHandle();
			/*if(g_current_ui == E_SCREEN_HOME)
			{
                ClearBottomLine();
				DrawTextAtLineTwo(grid_x,&gg_song_typeface_16[178+32*0x68], E_DRAW_LCD_TYPE_CHINESE_WORD);
				DrawTextAtLineTwo(grid_x+16,&gg_song_typeface_16[178+32*0x66], E_DRAW_LCD_TYPE_CHINESE_WORD);
				DrawTextAtLineTwo(grid_x+32,&gg_song_typeface_16[178+32*0x58], E_DRAW_LCD_TYPE_CHINESE_WORD);
				DrawTextAtLineTwo(grid_x+48,&gg_song_typeface_16[178+32*0x71], E_DRAW_LCD_TYPE_CHINESE_WORD);
				DrawTextAtLineTwo(grid_x+64,&gg_song_typeface_16[178+32*0x8b+16*0x0a], E_DRAW_LCD_TYPE_ASCII_CHAR);
				DrawTextAtLineTwo(grid_x+72,&gg_song_typeface_16[178+32*0x8b+16*0x00], E_DRAW_LCD_TYPE_ASCII_CHAR);
				DrawTextAtLineTwo(grid_x+80,&gg_song_typeface_16[178+32*0x8b+16*0x02], E_DRAW_LCD_TYPE_ASCII_CHAR);
			}*/
			g_alarm_status = (T_U16)e_alarm_id;
			DisplayUI(E_SCREEN_SHOW_SYSTEM_ERROR);
			break;
		case E_ALARM_ENCODER_ERROR_ID:
			g_alarm_status = (T_U16)e_alarm_id;
			DisplayUI(E_SCREEN_SHOW_SYSTEM_ERROR);
			break;
		case E_ALARM_SPI_COMM_FAIL_ID:
			g_alarm_status = (T_U16)e_alarm_id;
			DisplayUI(E_SCREEN_SHOW_SYSTEM_ERROR);
			break;
		case E_ALARM_MCU_ERROR_ID:
			//CtlKeyClearAllHandle();
			/*if(g_current_ui == E_SCREEN_HOME)
			{
                ClearBottomLine();
				DrawTextAtLineTwo(grid_x,&gg_song_typeface_16[178+32*0x68], E_DRAW_LCD_TYPE_CHINESE_WORD);
				DrawTextAtLineTwo(grid_x+16,&gg_song_typeface_16[178+32*0x66], E_DRAW_LCD_TYPE_CHINESE_WORD);
				DrawTextAtLineTwo(grid_x+32,&gg_song_typeface_16[178+32*0x58], E_DRAW_LCD_TYPE_CHINESE_WORD);
				DrawTextAtLineTwo(grid_x+48,&gg_song_typeface_16[178+32*0x71], E_DRAW_LCD_TYPE_CHINESE_WORD);
				DrawTextAtLineTwo(grid_x+64,&gg_song_typeface_16[178+32*0x8b+16*0x0a], E_DRAW_LCD_TYPE_ASCII_CHAR);
				DrawTextAtLineTwo(grid_x+72,&gg_song_typeface_16[178+32*0x8b+16*0x00], E_DRAW_LCD_TYPE_ASCII_CHAR);
				DrawTextAtLineTwo(grid_x+80,&gg_song_typeface_16[178+32*0x8b+16*0x03], E_DRAW_LCD_TYPE_ASCII_CHAR);
			}*/
			g_alarm_status = (T_U16)e_alarm_id;
			DisplayUI(E_SCREEN_SHOW_SYSTEM_ERROR);
			break;
		case E_ALARM_FRAM_ERROR_ID:
			//CtlKeyClearAllHandle();
			/*if(g_current_ui == E_SCREEN_HOME)
			{
                ClearBottomLine();
				DrawTextAtLineTwo(grid_x,&gg_song_typeface_16[178+32*0x68], E_DRAW_LCD_TYPE_CHINESE_WORD);
				DrawTextAtLineTwo(grid_x+16,&gg_song_typeface_16[178+32*0x66], E_DRAW_LCD_TYPE_CHINESE_WORD);
				DrawTextAtLineTwo(grid_x+32,&gg_song_typeface_16[178+32*0x58], E_DRAW_LCD_TYPE_CHINESE_WORD);
				DrawTextAtLineTwo(grid_x+48,&gg_song_typeface_16[178+32*0x71], E_DRAW_LCD_TYPE_CHINESE_WORD);
				DrawTextAtLineTwo(grid_x+64,&gg_song_typeface_16[178+32*0x8b+16*0x0a], E_DRAW_LCD_TYPE_ASCII_CHAR);
				DrawTextAtLineTwo(grid_x+72,&gg_song_typeface_16[178+32*0x8b+16*0x00], E_DRAW_LCD_TYPE_ASCII_CHAR);
				DrawTextAtLineTwo(grid_x+80,&gg_song_typeface_16[178+32*0x8b+16*0x04], E_DRAW_LCD_TYPE_ASCII_CHAR);
			}*/
			g_alarm_status = (T_U16)e_alarm_id;
			DisplayUI(E_SCREEN_SHOW_SYSTEM_ERROR);
			break;
		case E_ALARM_LOW_BATTERY_ID:
			g_alarm_status = (T_U16)e_alarm_id;		            
			//CtlKeyClearAllHandle();
			if(g_current_ui == E_SCREEN_HOME)
			{					//!µçÁ¿µÍ
				DisplayUI(E_SCREEN_HOME);
			}
            break;
		case E_ALARM_BATTERY_EXHAUSTION_ID:            
			g_alarm_status = (T_U16)e_alarm_id;
		    //CtlKeyClearAllHandle();
			if(g_current_ui == E_SCREEN_HOME)
			{					//!!Çë»»µç³Ø
				DisplayUI(E_SCREEN_HOME);
			}
			break;
		case E_ALARM_DOSE_EMPTY_ID:
            g_alarm_status = (T_U16)e_alarm_id;
            if(g_current_ui == E_SCREEN_HOME)
            {
				DisplayUI(E_SCREEN_HOME);
			}
		    break;
		case E_ALARM_OCCLUSION_ID:            
			g_alarm_status = (T_U16)e_alarm_id;
			//CtlKeyClearAllHandle();
			if(g_current_ui == E_SCREEN_HOME)
			{					//!!×èÈû
				DisplayUI(E_SCREEN_HOME);
			}
			break;
		case E_ALARM_DELI_SWITCH_OFF_ID:
			g_alarm_status = (T_U16)e_alarm_id;
            if(g_current_ui == E_SCREEN_HOME)
            {
				DisplayUI(E_SCREEN_HOME);
			}
			break;
		case E_ALARM_DOSE_NEAR_EMPTY_ID:
		    break;
		default:
			break;
	}
}

/***********************************************
* Description:
*   get high alarm information.
* Argument:
*
* Return:
*
************************************************/
T_U16 AlarmGetHighAlarmInformation(void)
{
    return g_alarm_bits.alarm_high_bits.alarm_bits;
}


/***********************************************
* Description:
*   get milldle alarm information.
* Argument:
*
* Return:
*
************************************************/
T_U16 AlarmGetMiddleAlarmInformation(void)
{
    return g_alarm_bits.alarm_middle_bits.alarm_bits;
}


/***********************************************
* Description:
*   get  low alarm information.
* Argument:
*
* Return:
*
************************************************/
T_U16 AlarmGetLowAlarmInformation(void)
{
    return g_alarm_bits.alarm_low_bits.alarm_bits;
}

/***********************************************
* Description:
*   get  remind alarm information.
* Argument:
*
* Return:
*
************************************************/
T_U16 AlarmGetRemindAlarmInformation(void)
{
    return g_alarm_bits.alarm_remind_bits.alarm_bits;
}


/***********************************************
* Description:
*   get all alarm information.
* Argument:
*
* Return:
*
************************************************/
T_U32 AlarmGetAllAlarmInformation(void)
{
    T_U32 alarm;
    T_U32 tmp;

    alarm =  g_alarm_bits.alarm_remind_bits.alarm_bits;
  
    tmp = g_alarm_bits.alarm_low_bits.alarm_bits;
    alarm =alarm|(tmp<<1);
	
    tmp = g_alarm_bits.alarm_middle_bits.alarm_bits;
    alarm =alarm|(tmp<<8);

    tmp = g_alarm_bits.alarm_high_bits.alarm_bits;
    alarm = alarm|(tmp<<16);

    return alarm;
}

T_U32 AlarmGetBatteryLowTime(void)
{
    return g_monitor_battery_low_time;
}


T_U32 AlarmGetBatteryDepleteTime(void)
{
    return g_monitor_battery_deplete_time;
}

TE_BOOLEAN AlarmGetBatteryLowFlag(void)
{
    return g_flag_battery_low;
}

void AlarmSetBatteryLowFlag(void)
{
    g_flag_battery_low = E_TRUE;
}

void AlarmClrBatteryLowFlag(void)
{
    g_flag_battery_low = E_FALSE;
}

