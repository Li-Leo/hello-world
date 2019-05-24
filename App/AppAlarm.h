/************************************************
* COPYRIGHT 2016.
*
* NAME: AppAlarm.h
*
* DESCRIPTION:
*   alarm module
*
* REVISION HISTORY:
*	Created by Kevin Shi at 06/20/2016
*
* END OF FILE HEADER
************************************************/

#ifndef APP_ALARM_H
#define APP_ALARM_H

#include "..\TypeDefine.h"
#include "..\Common.h"
#include "..\Kernel\CtlEvent.h"

/* alarm counts for all kinds of alarm level */
#define REMIND_ALARM_COUNT	3
#define LOW_ALARM_COUNT		2
#define MIDDLE_ALARM_COUNT	4
#define HIGH_ALARM_COUNT	6

/* alarm id */
typedef enum
{
	/* remind priority alarm id */
	E_ALARM_REMIND_DOSE_LEFT_LESS_THAN_INFU_VOL_ID,
	E_ALARM_REMIND_DAY_TOTAL_OVERRUN_ID,
	E_ALARM_REMIND_DAY_TOTAL_OUT_OF_RANGE_ID,
	
	/* low priority alarm id*/    
	//E_ALARM_FORGOT_OPERATION_ID,
	E_ALARM_LOW_BATTERY_ID,
	E_ALARM_DOSE_NEAR_EMPTY_ID,
	
	/* middle priority alarm id */
	E_ALARM_BATTERY_EXHAUSTION_ID,
	E_ALARM_DOSE_EMPTY_ID,
	E_ALARM_OCCLUSION_ID,
	E_ALARM_DELI_SWITCH_OFF_ID,

	/* high priority alarm id */
	E_ALARM_KEY_STUCK_ID,
	E_ALARM_ENCODER_ERROR_ID,
	E_ALARM_MCU_ERROR_ID,
	E_ALARM_MOTOR_ERROR_ID,
	E_ALARM_FRAM_ERROR_ID, 
	E_ALARM_SPI_COMM_FAIL_ID,

	E_ALARM_COUNT
}TE_ALARM_ID;

/* alarm level */
typedef enum
{
	E_ALARM_NONE,
	E_ALARM_REMIND,
	E_ALARM_LOW,
	E_ALARM_MIDDLE,
	E_ALARM_HIGH,
	E_ALARM_MAX    
} TE_ALARM_LEVEL;

/* alarm times */
typedef enum
{
	E_ALARM_BEEP_ONE_WITH_VIBR,
	E_ALARM_BEEP_ONE_NO_VIBR,
	E_ALARM_BEEP_TWO_WITH_VIBR,
	E_ALARM_BEEP_TWO_WITHOUT_VIBR,
	E_ALARM_TYPE_MAX    
} TE_ALARM_TYPE;

typedef union
{
	struct
	{
		T_U16 alarm_remind_dose_left:1;	/* hint while the dose left less than infu vol */
		T_U16 alarm_remind_dat_total_overrun:1;	/* hint while day total overrun */
		T_U16 alarm_remind_dat_total_out_range:1;	/* hint while day total out of range */
		T_U16 alarm_low_dose:1;			/* low dose alarm */ 		
		T_U16 na:12;
	} alarm_bits_struct;
	T_U16 alarm_bits;
} TU_ALARM_REMIND;


typedef union
{
	struct
	{
		T_U16 alarm_low_battery:1;		/* low battery alarm */
		T_U16 na:15;
	} alarm_bits_struct;
	T_U16 alarm_bits;
} TU_ALARM_LOW;

typedef union
{
	struct
	{
		T_U16 alarm_battery_exhaustion:1;	/* battery exhaustion alarm */
		T_U16 alarm_dose_empty:1;			/* dose empty alarm */
		T_U16 alarm_occlusion:1;			/* occlusion alarm */
		T_U16 alarm_deli_switch_off:1;		/* delivery switch off */                 
		T_U16 na:12;
	} alarm_bits_struct;
	T_U16 alarm_bits;
} TU_ALARM_MIDDLE;

typedef union
{
	struct
	{  
		T_U16 alarm_key_stuck:1;			/* key stuck */
		T_U16 alarm_encoder_error:1;		/* encoder error */
		T_U16 alarm_mcu_error:1;			/* mcu error */
		T_U16 alarm_motor_error:1;			/* motor error */
		T_U16 alarm_fram_error:1;			/* FRAM error */  
		T_U16 alarm_spi_comm_fail:1;		/* SPI comm fail */  
		T_U16 na:10;
	} alarm_bits_struct;
	T_U16 alarm_bits;
} TU_ALARM_HIGH;

typedef struct
{
	TU_ALARM_REMIND		alarm_remind_bits;
	TU_ALARM_LOW        alarm_low_bits;
	TU_ALARM_MIDDLE     alarm_middle_bits;
	TU_ALARM_HIGH       alarm_high_bits;  
} TS_ALARM_BITS;

/* -------------external functions ------------- */
void AlarmInit(void);
void AlarmSet(TE_ALARM_ID e_alarm_id);
void AlarmClear(TE_ALARM_ID e_alarm_id);
void AlarmClearAll(void);
TE_BOOLEAN AlarmIsSet(TE_ALARM_ID e_alarm_id);
//TE_BOOLEAN AlarmHaveSysError(void);
TE_BOOLEAN AlarmHaveHighError(void);
TE_BOOLEAN AlarmHaveMiddleError(void);
TE_BOOLEAN AlarmHaveLowError(void);
//TE_BOOLEAN AlarmHaveLowestError(void);
TE_BOOLEAN AlarmHaveRemind(void);
TE_ALARM_LEVEL AlarmGetHighestLevel(void);
TE_ALARM_LEVEL AlarmGetLevel(TE_ALARM_ID e_alarm_id);
TE_BOOLEAN AlarmIsLevelChange(void);

//T_U16 AlarmGetSysAlarmInformation(void);
T_U16 AlarmGetHighAlarmInformation(void);
T_U16 AlarmGetMiddleAlarmInformation(void);
T_U16 AlarmGetLowAlarmInformation(void);
T_U16 AlarmGetRemindAlarmInformation(void);
T_U32 AlarmGetAllAlarmInformation(void);

T_U32 AlarmGetBatteryLowTime(void);
T_U32 AlarmGetBatteryDepleteTime(void);
TE_BOOLEAN AlarmGetBatteryLowFlag(void);
void AlarmSetBatteryLowFlag(void);
void AlarmClrBatteryLowFlag(void);

TE_ALARM_ID AlarmShowFirstOfHighestPriority(void);

void AlarmPlay(TE_ALARM_TYPE type);
void AlarmPlayEx(TE_ALARM_TYPE type, TE_BOOLEAN is_play_at_once);
void AlarmShow(TE_ALARM_ID e_alarm_id);

#endif /* APP_ALARM_H */

