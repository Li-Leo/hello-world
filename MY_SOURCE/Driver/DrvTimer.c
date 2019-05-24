/*******************************************************************************
**�ļ���:DrvTimer.c
**
**����:
**
**����:HJH
**
**����:2016-04-18
**
**�޸�����:
**
**����:
**
**����:
**
*******************************************************************************/

#include "DrvTimer.h"
#include "..\Kernel\CtlTime.h"
#include "AppMonitor.h"
#include "CtlTimer.h"
#include "AppDelivery.h"
#include "stdio.h"
#include "common.h"
#include "DrvClk.h"
#include "DrvRTC.h"

//#define CHECK_RTC_INTERVAL_TIME_MS 60000
#define TIMER_DEVIATION_SECS_EACH_MINUTE 10
#ifdef USE_EXTERNAL_CLOCK_FOR_LOW_POWER
#define WAKE_UP_INTERVAL_TIME_MS 125
#else
#define WAKE_UP_INTERVAL_TIME_MS 100
#endif


T_U32 g_time_ms_elapse = 0;
T_S32 g_timer_pass_count = 0;
T_S32 g_timer_next_check_rtc_count = 0;
T_S32 g_auto_wakeup_timer_ms = 0;
//only use the week, hour, minute, second
TS_TIME_INFO g_auto_wakeup_time;

T_U32 g_time_us_tick = 0;  //one tick 10us
T_U32 volatile g_time_us_total_time = 0;
T_U32 g_time_delivery_counter = 0;
T_U32 g_check_motor_run_time = 0;
T_U32 g_check_motor_run_time_last = 0;


T_U32 g_time_delivery_counter_outer = 0;
T_U32 g_time_delivery_counter_inner = 0;

T_U32  g_time_us_total_time_500 = 0;
T_U32  g_time_us_total_time_1000 = 0;
T_U32  g_time_us_total_time_2000 = 0;
T_U32  g_time_us_total_time_3000 = 0;
T_U32  g_time_us_total_time_4000 = 0;

T_U32 g_time_delivery_counter_500 = 0;
T_U32 g_time_delivery_counter_1000 = 0;
T_U32 g_time_delivery_counter_2000 = 0;
T_U32 g_time_delivery_counter_3000 = 0;
T_U32 g_time_delivery_counter_4000 = 0;

/*******************************************************************************
**������:void DrvTimerInital(void)
**
**����:���ö�ʱ��TA0��TA1��TA2��TA3��TB0�������������ݰ���
**
**���ߣ�HJH
**
**���ڣ�2016-04-18
**
**�޸����ݣ�
**
**���ߣ�
**
**���ڣ�
*******************************************************************************/
void DrvTimerInitial(void)
{
  //TA0CTL = TASSEL__SMCLK|ID_3;//��ʱ��TA0��ʱ��ԴΪSMCLK��ʹ�ܶ�ʱ���жϣ�ֹͣ��
                              //ʱ������
/******************************************************************************/  
  TA1CTL |= TASSEL__ACLK;     //��ʱ��TA1��ʱ��ԴΪACLK��ʹ�ܶ�ʱ���жϣ�ֹͣ��
                              //ʱ������
  TA1CCTL0 |= CCIE;//ʹ�ܶ�ʱ��TA1CCR0.CCIFG�ж�
/******************************************************************************/  
  TA2CTL |= TASSEL__SMCLK|ID_3;//��ʱ��TA2��ʱ��ԴΪSMCLK��ID_3��ʾ2^3��Ƶ, ʹ�ܶ�ʱ���жϣ�ֹͣ��
                               //ʱ������
  TA2CCTL0 |= CCIE;            //ʹ�ܶ�ʱ��TA2CCR0.CCIFG�ж�
/******************************************************************************/  
  TA3CTL |= TASSEL__SMCLK;     //��ʱ��TA3��ʱ��ԴΪSMCLK��ʹ�ܶ�ʱ���жϣ�ֹͣ��
                               //ʱ������      TASSEL__SMCLK = 1M
  TA3CCTL0 |= CCIE;            //ʹ�ܶ�ʱ��TA3CCR0.CCIFG�ж�
  /******************************************************************************/  
  TB0CTL |= TBSSEL_2;          //��ʱ��TA1��ʱ��ԴΪSMCLK��ʹ�ܶ�ʱ���жϣ�ֹͣ��
                               //ʱ������
  TB0CCTL0 |= CCIE;            //ʹ�ܶ�ʱ��TA1CCR0.CCIFG�ж�

}

void DrvTimerCheckAndSetAutoWakeupTimer()
{
    T_S32 diff_secs;

    if(g_auto_wakeup_time.week == 0)
    {
        g_auto_wakeup_timer_ms = MAX_T_S32;
        g_timer_next_check_rtc_count = MAX_T_S32;
    }
    else
    {
        Ds1302TimeRead(); 
        diff_secs = DiffDateTimeInOneDay(&g_auto_wakeup_time, &g_current_time);
    	M_Trace("time: week:%d, %02d:%02d:%02d\n", (T_S16)g_current_time.week-1,
                (T_S16)g_current_time.hour, (T_S16)g_current_time.minute, (T_S16)g_current_time.second);
        M_Trace("change the sleep ms from %ld to %ld\n", g_auto_wakeup_timer_ms, diff_secs*1000);        
        //set the wakeup time ms
        g_auto_wakeup_timer_ms = diff_secs*1000;
        //restart the count
        g_timer_pass_count = 0;
		//����ACLK���ܴ�,����ÿһ��ʱ����һ��RTC
		if(diff_secs>=60){
			//ʣ��ʱ�����ƫ��>=ÿ�������ƫ��, ÿ���1���Ӽ��һ��
        	g_timer_next_check_rtc_count = (60-TIMER_DEVIATION_SECS_EACH_MINUTE)*1000L;
		}else if(diff_secs>=60/TIMER_DEVIATION_SECS_EACH_MINUTE){
			//ʣ��ʱ�����ƫ��>=1��, �����´μ��RTCʱ��
			g_timer_next_check_rtc_count = (60-TIMER_DEVIATION_SECS_EACH_MINUTE)*diff_secs/60*1000;
		}else{
			//ʣ��ʱ������ƫ��<1��, ���ټ��RTC
			g_timer_next_check_rtc_count = MAX_T_S32;
		}
        
    }    
}
/*******************************************************************************
**������:void DrvTimerEnable(T_U16 timer_count,TE_TIMER_CHANNEL cha)
**
**����:����ĳ����ʱ�������趨��ʱʱ����timer_count:��Ҫ��ʱ��ʱ������ʱ��1msλ��
**
**����ʱ��λ����timer_count = 2�����ʾ��ʱ2ms��cha:���ڶ�ʱ�Ķ�ʱ��ͨ����
**
**���ߣ�HJH
**
**���ڣ�2016-04-18
**
**�޸����ݣ�
**
**���ߣ�
**
**���ڣ�
*******************************************************************************/
void DrvTimerEnable(TE_TIMER_CHANNEL cha)
{
    T_U16 temp = MSECS_PER_TICK;//��Ҫд�붨ʱ�������Ĵ�����ֵ

    switch(cha)
    {
        case E_TIMER_CHANNEL_A0:
        {
            TA0CCR0 = temp * 125 - 1;//���ö�ʱ��TA0����ֵ
            TA0CTL |= MC_1;         //��������ʱ
            TA0CCTL0 |= CCIE;       //ʹ�ܶ�ʱ��TA0CCR0.CCIFG�ж�
        }
        break;

        case E_TIMER_CHANNEL_A1:
        {
            g_timer_pass_count = 0;
            DrvTimerCheckAndSetAutoWakeupTimer();
            TA1R = 0;
#ifdef USE_EXTERNAL_CLOCK_FOR_LOW_POWER            
            TA1CCR0 = 4096;      //���ö�ʱ��TA1����ֵ, ��ʱ��Ƶ��Ϊ32768hz, ��Ϊ125ms������
#else
            TA1CCR0 = 1*940;      //���ö�ʱ��TA1����ֵ, ��ʱ��Ƶ��Ϊ9.4khz, ��Ϊ100ms������
#endif            
            TA1CTL |= MC_1;         //��������ʱ
            TA1CCTL0 |= CCIE;       //ʹ�ܶ�ʱ��TA0CCR0.CCIFG�ж�
        }
        break;

        case E_TIMER_CHANNEL_A2:
        {
            TA2CCR0 = temp*125-1;   //���ö�ʱ��TA2����ֵ
            TA2CTL |= MC_1;         //��������ʱ
            TA2CCTL0 |= CCIE;       //ʹ�ܶ�ʱ��TA0CCR0.CCIFG�ж�
        }
        break;

        case E_TIMER_CHANNEL_A3:    
        {
            TA3CCR0 = 10 - 1;            //���ö�ʱ��TA3����ֵ
            TA3CTL |= MC_1;         //��������ʱ
            TA3CCTL0 |= CCIE;       //ʹ�ܶ�ʱ��TA0CCR0.CCIFG�ж�
        }
        break;

        case E_TIMER_CHANNEL_B0:
        {
            TB0CCR0 = temp*8000;    //���ö�ʱ��TB0����ֵ
            TB0CTL |= MC_1;         //��������ʱ
            TB0CCTL0 |= CCIE;       //ʹ�ܶ�ʱ��TA0CCR0.CCIFG�ж�
        }
        break;

        default:
            break;
    }
}

/*******************************************************************************
**������:void DrvTimerDisable(TE_TIMER_CHANNEL cha)
**
**����:��ֹĳ����ʱ���������������ʱ���жϱ�־
**
**���ߣ�HJH
**
**���ڣ�2016-04-18
**
**�޸����ݣ�
**
**���ߣ�
**
**���ڣ�
*******************************************************************************/
void DrvTimerDisable(TE_TIMER_CHANNEL cha)
{
   switch(cha)
  {
    case E_TIMER_CHANNEL_A0:
      {
        TA0CTL &= ~MC_1;    //ֹͣ����ʱ
        TA0CCTL0 &= ~CCIE;  //��ֹ��ʱ��TA0CCR0.CCIFG�ж�
        TA0CCTL0 &= ~CCIFG; //�����ʱ���жϱ�־
      }
      break;

    case E_TIMER_CHANNEL_A1:
      {
        TA1CTL &= ~MC_1;//ֹͣ����ʱ
        TA1CCTL0 &= ~CCIE;//��ֹ��ʱ��TA1CCR0.CCIFG�ж�
        TA1CCTL0 &= ~CCIFG;//�����ʱ���жϱ�־
      }
      break;

    case E_TIMER_CHANNEL_A2:
      {
        TA2CTL   &= ~MC_1;//ֹͣ����ʱ
        TA2CCTL0 &= ~CCIE;//��ֹ��ʱ��TA2CCR0.CCIFG�ж�
        TA2CCTL0 &= ~CCIFG;//�����ʱ���жϱ�־
      }
      break;

    case E_TIMER_CHANNEL_A3:
      {
        TA3CTL   &= ~MC_1;//ֹͣ����ʱ
        TA3CCTL0 &= ~CCIE;//��ֹ��ʱ��TA3CCR0.CCIFG�ж�
        TA3CCTL0 &= ~CCIFG;//�����ʱ���жϱ�־
      }
      break;

    case E_TIMER_CHANNEL_B0:
      {
        TB0CTL   &= ~MC_1;//ֹͣ����ʱ
        TB0CCTL0 &= ~CCIE;//��ֹ��ʱ��TB0CCR0.CCIFG�ж�
        TB0CCTL0 &= ~CCIFG;//�����ʱ���жϱ�־
      }
      break;

    default:
      break;
  }
}
/*******************************************************************************
**������:__interrupt void TimerA0Isr(void)
**
**����:��ʱTimer_A TA0�жϷ�������
**
**���ߣ�HJH
**
**���ڣ�2016-04-19
**
**�޸����ݣ�
**
**���ߣ�
**
**���ڣ�
*******************************************************************************/
#pragma vector = TIMER0_A0_VECTOR
__interrupt void TimerA0Isr(void)
{
	//CtlTimeOnOneTick();
    //g_time_ms_elapse++;
	//if (g_time_ms_elapse % 5 == 0)
		//CtlKeyScan();
#ifdef _DEBUG		
    LPM3_EXIT;
#endif
}
/*******************************************************************************
**������:__interrupt void TimerA1Isr(void)
**
**����:��ʱTimer_A TA1�жϷ�������
**
**���ߣ�HJH
**
**���ڣ�2016-04-19
**
**�޸����ݣ�
**
**���ߣ�
**
**���ڣ�
*******************************************************************************/
#pragma vector = TIMER1_A0_VECTOR
__interrupt void TimerA1Isr(void)
{
	g_timer_pass_count += WAKE_UP_INTERVAL_TIME_MS;
    if(g_timer_pass_count<0)
    {
       g_timer_pass_count = 0; 
    }
    //printf("curr: %lu\n", g_timer_pass_count);

    //handle the alarm timer 
	CtlTimeOnOneTick(WAKE_UP_INTERVAL_TIME_MS);   
	CtlTimerHandle(LOW_POWER_MODE_TIMER);

    //check if need check RTC
	if(g_timer_pass_count>=g_timer_next_check_rtc_count)
	{
        //check RTC and set the new destination
        DrvTimerCheckAndSetAutoWakeupTimer(); 
	}

    //check if arrive the destination
	if (g_timer_pass_count >= g_auto_wakeup_timer_ms)
	{
		__bic_SR_register_on_exit(LPM3_bits);
	}
}
/*******************************************************************************
**������:__interrupt void TimerA2Isr(void)
**
**����:��ʱTimer_A TA2�жϷ�������
**
**���ߣ�HJH
**
**���ڣ�2016-04-19
**
**�޸����ݣ�
**
**���ߣ�
**
**���ڣ�
*******************************************************************************/
#pragma vector = TIMER2_A0_VECTOR
__interrupt void TimerA2Isr(void)
{
  CtlTimeOnOneTick(MSECS_PER_TICK);

  LPM1_EXIT;
}
/*******************************************************************************
**������:__interrupt void TimerA3Isr(void)
**
**����:��ʱTimer_A TA3�жϷ�������
**
**���ߣ�HJH
**
**���ڣ�2016-04-19
**
**�޸����ݣ�
**
**���ߣ�
**
**���ڣ�
*******************************************************************************/
#pragma vector = TIMER3_A0_VECTOR
__interrupt void TimerA3Isr(void)
{
  //DrvTimerDisable(E_TIMER_CHANNEL_A3);
  g_time_us_tick++;

#ifdef _DEBUG		
 LPM3_EXIT;
#endif  
}
/*******************************************************************************
**������:__interrupt void TimerB0Isr(void)
**
**����:��ʱTimer_B TB0�жϷ�������
**
**���ߣ�HJH
**
**���ڣ�2016-04-19
**
**�޸����ݣ�
**
**���ߣ�
**
**���ڣ�
*******************************************************************************/
#pragma vector = TIMER0_B0_VECTOR
__interrupt void TimerB0Isr(void)
{
  DrvTimerDisable(E_TIMER_CHANNEL_B0);

#ifdef _DEBUG		
 LPM3_EXIT;
#endif  
}
