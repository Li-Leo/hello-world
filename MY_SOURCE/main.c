/********************************
********    Master MCU   ********
********************************/
/*******************************************************************************
**文件名:main.c
**
**功能:
**
**作者:
**
**日期:2016-03-01
**
*******************************************************************************/
#include <msp430fr5989.h>
#include "App\Init.h"
#include "App\AppuiScreen.h"
#include "CtlKey.h"
#include "CtlTime.h"
#include "CtlTimer.h"
#include "..\Kernel\CtlDisplay.h"
#include "..\Kernel\CtlDelivery.h"
#include "..\Kernel\CtlData.h"
#include "..\Kernel\Fonts.h"
#include "..\Driver\DrvLcd.h"
#include "..\Driver\DrvPower.h"
#include "..\Driver\DrvRtc.h"
#include "CtlSpiComm.h"
#include "CtlEvent.h"
#include "DrvUart.h"
#include "CtlComm.h"
#include "AppDelivery.h"
#include "intrinsics.h"
#include "CtlMotor.h"
#include "AppMonitor.h"
#include "AppAlarm.h"
#include "CtlCommon.h"
#include "string.h"
#include "AppCommon.h"
#include "AppUiHandle.h"

#define MOTOR_RUN_TIME_MAX_ERROR_COUNT 2
#define OCCLUSION_STATE_BUFF_SIZE      5

//may be SYSRSTIV_BOR,SYSRSTIV_RSTNMI,SYSRSTIV_MPUSEGPIFG
T_U16 g_power_up_cause;
TE_BOOLEAN g_is_reset_by_smcu = E_FALSE;

extern TE_BOOLEAN g_monitor_is_battery_voltage_empty;


//check previous run is normal power off
void CheckPreviousRunState()
{
	TE_BOOLEAN is_need_continue_temp_base = E_FALSE;
	TE_BOOLEAN is_need_continue_meal_bolus = E_FALSE;
	T_U32 last_infusion_encoder = 0;

    //check if is infusioning at last power on
	if (g_deliver_is_running)
	{
        g_deliver_is_running = 0;
        SaveSysPara(E_SYSTEM_PARA_DELI_IS_RUNNING, 0);    

        if(g_motor_position<=g_deliver_start_motor_pos)
        {
            if(g_deliver_start_motor_pos>=g_motor_position)
            {
                last_infusion_encoder = g_deliver_start_motor_pos - g_motor_position;
            }
        }
	}
	
    //check if abnormal reset
	if (g_mcu_abnormal_reset_state == E_TRUE)
	{
        M_CommonPrint("not safety power off last time\n");
		SaveRec(E_RECORD_OPERATION, E_OPERATION_ALARM_SYSTEM_ABNORMAL_RESTART, g_power_up_cause);
	}
	else
	{
		is_need_continue_temp_base = E_TRUE;
		g_mcu_abnormal_reset_state = E_TRUE;
		SaveSysPara(E_SYSTEM_PARA_SIN_RST_FLAG, 0);
	}

	if(g_is_reset_by_smcu){
		is_need_continue_temp_base = E_TRUE;
		is_need_continue_meal_bolus = E_TRUE;
		if(last_infusion_encoder >0){
			//if reset by smcu, record last infusion to day total
			RecAddToDayTotal(EncoderToDose(g_sys_para_thickness, last_infusion_encoder));
		}
	}

	//检查是否需要继续输注临基
	if(is_need_continue_temp_base){
		if (g_temp_basic_infusion_info.is_infusioning){
			DeliveryUpdateInfusionInfo(g_temp_basic_infusion_info.infusioning_dose_encoder,
				g_temp_basic_infusion_info.infusioning_dose_encoder,
				E_INFUSION_TEMP_BASE, g_temp_basic_infusion_info.infusioning_dose_encoder);
		}
	}else{
		if(DeliveryIsTempBaseValid()){
			DeliveryStopTempBase();
		}
	}

	//检查是否需要继续输注餐前
	if(is_need_continue_meal_bolus){
		if (g_meal_bolus_infusion_info.is_infusioning){
			DeliveryUpdateInfusionInfo(g_meal_bolus_infusion_info.infusioning_dose_encoder,
				g_meal_bolus_infusion_info.infusioning_dose_encoder,
				E_INFUSION_MEAL_BOLUS, last_infusion_encoder);
		}
	}else{
		if(DeliveryIsMealBolusValid()){
			DeliveryStopMealBolus();
		}
	}	
	
    if (g_occlusion_alarm_is_set== E_TRUE)
	{
		AlarmSet(E_ALARM_OCCLUSION_ID);
		MonitorCheckAlarm();
	}

	
}


__intrinsic int __low_level_init(void)
{

  WDTCTL = WDTPW + WDTHOLD;     //关看门狗 

   /*
  * Return value:
  *
  *  1 - Perform data segment initialization.
  *  0 - Skip data segment initialization.
  */

  return 1;
}

void WaitSMCUPowerUpTimerout(){
  CtlTimerClear(E_TIMER_WAIT_SMCU_POWER_UP);
  CheckPreviousRunState();
  
  SetNextDeliTimePoint();
  CtlTimerSet(E_TIMER_NO_OPERATION_SLEEP, NO_OPERATION_SLEEP_MS);	//NO_OPERATION_SLEEP_MS

  DisplayUI(E_SCREEN_HOME);
  CtlEventPost(E_EVENT_ENTER_STOP_STATE, 1);
  //monitor slave
  CtlTimerBindHandle(E_TIMER_EXTERN_MONITOR_START, ExternSysMonitorStart);
  CtlTimerSet(E_TIMER_EXTERN_MONITOR_START, EXTERN_SYSMONITOR_START_DELAY);

  
#if 0  //when debug, you can enable it to let amcu not monitor
  //let slave not monitor me
  SendSpiCommandToSlave(HEAD_SC, COMM_WD, 0);
  //stop monitor slave
  CtlTimerClear(E_TIMER_EXTERN_MONITOR_START);
  ExternSysMonitorStop();
#endif
}

//等待首次电池采样结束
void WaitFirstBatteryStable(){
	//将电池采样周期设为默认值
  	g_battery_stabilize_min_time_ms = BATTERY_STABILIZE_MIN_TIME_MS;
	g_battery_stabilize_min_count = BATTERY_STABILIZE_MIN_COUNT;
}

T_U8 get_sum(T_U8 *buff, T_U8 len){
    T_U16 i;
    T_U8 sum = 0;
    for(i = 0; i < len; i++){
        sum += *buff++;
    }
    return sum;
}

void main()
{
#if USE_INFUSION_TIME_TO_CHECK_OCCLUSION
    //used for occlusion check
    T_U8 occlusion_state_buff[OCCLUSION_STATE_BUFF_SIZE] = {0};
    T_U8 occlusion_state_buff_index = 0;
    T_U32 occlusion_runtime_threshold;
    T_U8 occlusion_runtime_threshold_max_error = 0;
#endif
	
  WDTCTL = WDTPW + WDTHOLD;     //关看门狗
  PM5CTL0 &= ~LOCKLPM5; 
  __enable_interrupt();
  g_power_up_cause = SYSRSTIV;
  //P1IE &= ~BIT1;
  g_batt_number = 3000;
  Initial();
  //g_motor_position = 152000;
  //SaveMotorPos(1);
  M_Trace("A-PumpV1.8 master start\n");
  M_Trace("power up cause:0x%X\n",g_power_up_cause);  
  M_Trace("checksum: 0x%lX\n",AppCommonGetSelfCRC32());  



  CtlSetCurrentMode(E_SYSTEM_MODE_NORMAL);
  if(g_mcu_abnormal_reset_state && g_power_up_cause == SYSRSTIV_BOR)
  {
  	//Brown-out Reset, need show battery quickly
  	g_battery_stabilize_min_time_ms = 1000;
	g_battery_stabilize_min_count = 10;
	CtlTimerBindHandle(E_TIMER_WAIT_FIRST_BATTERY_STABLE, WaitFirstBatteryStable);
  	CtlTimerSet(E_TIMER_WAIT_FIRST_BATTERY_STABLE, 5000);
  }else{
  	//ShowStartupPage();
  }
  ShowStartupPage();
  
  // wake up slave MCU(rising edge)
  PULSE_SIGNAL_LOW;
  P1OUT |= BIT4;    
  P1OUT &= ~BIT4;

  CtlTimerBindHandle(E_TIMER_WAIT_SMCU_POWER_UP, WaitSMCUPowerUpTimerout);
  CtlTimerSet(E_TIMER_WAIT_SMCU_POWER_UP, 50);

  PulseSignalSendStart();
  //tell slaver power up
  SendSpiCommandToSlave(HEAD_SC, COMM_PU, 0);

  #if 0  //when debug, you can enable it to let amcu not monitor
  //let slave not monitor me
  SendSpiCommandToSlave(HEAD_SC, COMM_WD, 0);
  #endif
  while(1)
  {
    CtlEventHandleAll();
    if (!CtlTimeIsTickHandled())
    {
      CtlTimeSetTickHandled(E_TRUE);
      CtlTimerHandle(ALL_TIMER);
    }

    if(g_encoder_is_monitor)
    {
		if(g_encoder_need_stop_flag==1){
	        if(!CtlMotorIsStoping() && CtlMotorIsRunning())
	        {
	            CtlMotorStopEx(E_MOTOR_STOP, g_motor_stop_cause, 0);
	        }
        	g_encoder_is_monitor = 0; 
		}
#if USE_MOTOR_SPEED_TO_CHECK_OCCLUSION		
		else if(g_encoder_is_speed_ready){
			g_encoder_is_speed_ready = E_FALSE;
			MonitorCheckMotorSpeed(E_FALSE);
			AppCalOcclRemindScreenOnGetMotorSpeed(E_FALSE);
			
		}
#endif		
    }

	if(g_encoder_pos_changed_flag){
		g_encoder_pos_changed_flag = E_FALSE;
		SaveMotorPos(E_FALSE);
	}

#if USE_INFUSION_TIME_TO_CHECK_OCCLUSION	
    if(g_is_get_new_motor_run_time == E_TRUE) {
        g_is_get_new_motor_run_time = E_FALSE;

        if(g_encoder_expect_counter >= 120 && GetCurrentUI() != E_SCREEN_CAL_PRESSURE_TIME_REMIND && GetCurrentUI() != E_SCREEN_CAL_PRESSURE_MAX_TIME_REMIND && GetCurrentUI() != E_SCREEN_CAL_LOC_SWI_REMIND) {
            if (g_check_motor_run_time_last != 0) {
                occlusion_runtime_threshold = GetOcclusionThreshold(g_encoder_counter_for_occlusion, E_FALSE);
                M_MotorPrint("threshold=%lu,runtime=%lu, total=%lu\n", occlusion_runtime_threshold, g_check_motor_run_time - g_check_motor_run_time_last, g_check_motor_run_time);
                if (g_check_motor_run_time - g_check_motor_run_time_last >= occlusion_runtime_threshold) {
                    occlusion_runtime_threshold_max_error++;
                } else {
                    occlusion_runtime_threshold_max_error = 0;
                }
            }

            if (CtlMotorDirection() == E_MOVE_FORWARD && occlusion_runtime_threshold_max_error >= 3){
                if (g_monitor_is_battery_voltage_empty) {
                    AlarmSet(E_ALARM_BATTERY_EXHAUSTION_ID);
                    M_CommonPrint("battery empty!\n");
                } else {
                    AlarmSet(E_ALARM_OCCLUSION_ID);
                    SaveRec(E_RECORD_OPERATION, E_OPERATION_PARA, occlusion_runtime_threshold);
                    SaveRec(E_RECORD_OPERATION, E_OPERATION_PARA2, g_check_motor_run_time - g_check_motor_run_time_last);
                    occlusion_runtime_threshold_max_error = 0;
                    M_MotorPrint("Blocked!\n");
                }
            }
            g_check_motor_run_time_last = g_check_motor_run_time;
        } else if (g_encoder_expect_counter >= 9 && GetCurrentUI() != E_SCREEN_CAL_PRESSURE_TIME_REMIND && GetCurrentUI() != E_SCREEN_CAL_PRESSURE_MAX_TIME_REMIND && GetCurrentUI() != E_SCREEN_CAL_LOC_SWI_REMIND) {

            occlusion_runtime_threshold = GetOcclusionThreshold(g_encoder_counter_for_occlusion, E_TRUE);

            M_MotorPrint("threshold=%lu,runtime=%lu,encoder=%lu\n", occlusion_runtime_threshold, g_check_motor_run_time, g_encoder_counter_for_occlusion);

            if (g_check_motor_run_time > occlusion_runtime_threshold) {
                //printf("encoder=%lu,expect=%lu,threshold=%lu,runtime=%lu\n", g_encoder_counter,g_encoder_expect_counter, GetMotorRunTimeThreshold(g_encoder_counter),g_check_motor_run_time);
                occlusion_state_buff[occlusion_state_buff_index] = 1;
            } else {
                occlusion_state_buff[occlusion_state_buff_index] = 0;
            }

            if (occlusion_state_buff_index >= OCCLUSION_STATE_BUFF_SIZE - 1)
                occlusion_state_buff_index = 0;
            else
                occlusion_state_buff_index++;

            //check alarm
            if (CtlMotorDirection() == E_MOVE_FORWARD && get_sum(occlusion_state_buff, OCCLUSION_STATE_BUFF_SIZE) >= MOTOR_RUN_TIME_MAX_ERROR_COUNT) {
                if (g_monitor_is_battery_voltage_empty) {
                    AlarmSet(E_ALARM_BATTERY_EXHAUSTION_ID);
                    M_CommonPrint("battery empty!\n");
                } else {
                    AlarmSet(E_ALARM_OCCLUSION_ID);
                    SaveRec(E_RECORD_OPERATION, E_OPERATION_PARA, occlusion_runtime_threshold);
                    SaveRec(E_RECORD_OPERATION, E_OPERATION_PARA2, g_check_motor_run_time);
                    M_MotorPrint("Blocked!\n");
                    memset(occlusion_state_buff, 0 , sizeof(occlusion_state_buff));
                }
            }
        }
    }
#endif

    if(DisplayIsDirty())
    {
        DisplayFlushDirty();
    }
    
    //handle new received data from usart
    if (UartGetNewDataFlag(E_UART0))
    {
        UartClearNewDataFlag(E_UART0);
        CommHandleNewReceiveData(E_COMM_PC_UART);
    }
    if (UartGetNewDataFlag(E_UART1))
    {
        UartClearNewDataFlag(E_UART1);
        CommHandleNewReceiveData(E_COMM_INTERNAL_UART);
    }

    if(CtlEventIsEmpty() && !DisplayIsDirty() &&
       CtlTimeIsTickHandled() )
    {   
        __bis_SR_register(LPM1_bits + GIE);      // LPM1, add GIE for debugger
    }
  }
  
}

#pragma vector = SYSNMI_VECTOR//非屏蔽中断
__interrupt void SYSNMI_ISR(void)
{
    switch (__even_in_range(SYSSNIV, SYSSNIV_CBDIFG)) {
    case SYSSNIV_NONE:       break;
    case SYSSNIV_SVS:        break;
    case SYSSNIV_UBDIFG:     break;
    case SYSSNIV_ACCTEIFG:   break;
    case SYSSNIV_MPUSEGPIFG: break;
    case SYSSNIV_MPUSEGIIFG: break;
    case SYSSNIV_MPUSEG1IFG:         // segment 1
    case SYSSNIV_MPUSEG2IFG:                // segment 2
    case SYSSNIV_MPUSEG3IFG:          // segment 3

        //write can not write area
        printf("Fault error: violation!\n");
        
        // Clear violation interrupt flag
        //MPU_clearInterruptFlag(__MSP430_BASEADDRESS_MPU__, MPUSEG2IFG);

        M_Assert(0);
        while (1);
        break;
    case SYSSNIV_VMAIFG:     break;
    case SYSSNIV_JMBINIFG:   break;
    case SYSSNIV_JMBOUTIFG:  break;
    case SYSSNIV_CBDIFG:     break;
    default:                 break;
    }
}

