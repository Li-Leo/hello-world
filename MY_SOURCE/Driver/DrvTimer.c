/*******************************************************************************
**文件名:DrvTimer.c
**
**功能:
**
**作者:HJH
**
**日期:2016-04-18
**
**修改内容:
**
**作者:
**
**日期:
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
**函数名:void DrvTimerInital(void)
**
**功能:配置定时器TA0，TA1，TA2，TA3和TB0，具体配置内容包括
**
**作者：HJH
**
**日期：2016-04-18
**
**修改内容：
**
**作者：
**
**日期：
*******************************************************************************/
void DrvTimerInitial(void)
{
  //TA0CTL = TASSEL__SMCLK|ID_3;//定时器TA0的时钟源为SMCLK，使能定时器中断，停止定
                              //时器工作
/******************************************************************************/  
  TA1CTL |= TASSEL__ACLK;     //定时器TA1的时钟源为ACLK，使能定时器中断，停止定
                              //时器工作
  TA1CCTL0 |= CCIE;//使能定时器TA1CCR0.CCIFG中断
/******************************************************************************/  
  TA2CTL |= TASSEL__SMCLK|ID_3;//定时器TA2的时钟源为SMCLK，ID_3表示2^3分频, 使能定时器中断，停止定
                               //时器工作
  TA2CCTL0 |= CCIE;            //使能定时器TA2CCR0.CCIFG中断
/******************************************************************************/  
  TA3CTL |= TASSEL__SMCLK;     //定时器TA3的时钟源为SMCLK，使能定时器中断，停止定
                               //时器工作      TASSEL__SMCLK = 1M
  TA3CCTL0 |= CCIE;            //使能定时器TA3CCR0.CCIFG中断
  /******************************************************************************/  
  TB0CTL |= TBSSEL_2;          //定时器TA1的时钟源为SMCLK，使能定时器中断，停止定
                               //时器工作
  TB0CCTL0 |= CCIE;            //使能定时器TA1CCR0.CCIFG中断

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
		//由于ACLK误差很大,所以每一段时间检测一次RTC
		if(diff_secs>=60){
			//剩余时间最大偏差>=每分钟最大偏差, 每差不多1分钟检测一次
        	g_timer_next_check_rtc_count = (60-TIMER_DEVIATION_SECS_EACH_MINUTE)*1000L;
		}else if(diff_secs>=60/TIMER_DEVIATION_SECS_EACH_MINUTE){
			//剩余时间最大偏差>=1秒, 计算下次检测RTC时间
			g_timer_next_check_rtc_count = (60-TIMER_DEVIATION_SECS_EACH_MINUTE)*diff_secs/60*1000;
		}else{
			//剩余时间的最大偏差<1秒, 不再检测RTC
			g_timer_next_check_rtc_count = MAX_T_S32;
		}
        
    }    
}
/*******************************************************************************
**函数名:void DrvTimerEnable(T_U16 timer_count,TE_TIMER_CHANNEL cha)
**
**功能:开启某个定时器，并设定定时时长。timer_count:需要定时的时长，定时以1ms位基
**
**本定时单位，如timer_count = 2，则表示定时2ms；cha:用于定时的定时器通道号
**
**作者：HJH
**
**日期：2016-04-18
**
**修改内容：
**
**作者：
**
**日期：
*******************************************************************************/
void DrvTimerEnable(TE_TIMER_CHANNEL cha)
{
    T_U16 temp = MSECS_PER_TICK;//将要写入定时器计数寄存器的值

    switch(cha)
    {
        case E_TIMER_CHANNEL_A0:
        {
            TA0CCR0 = temp * 125 - 1;//设置定时器TA0计数值
            TA0CTL |= MC_1;         //启动增计时
            TA0CCTL0 |= CCIE;       //使能定时器TA0CCR0.CCIFG中断
        }
        break;

        case E_TIMER_CHANNEL_A1:
        {
            g_timer_pass_count = 0;
            DrvTimerCheckAndSetAutoWakeupTimer();
            TA1R = 0;
#ifdef USE_EXTERNAL_CLOCK_FOR_LOW_POWER            
            TA1CCR0 = 4096;      //设置定时器TA1计数值, 定时器频率为32768hz, 设为125ms的周期
#else
            TA1CCR0 = 1*940;      //设置定时器TA1计数值, 定时器频率为9.4khz, 设为100ms的周期
#endif            
            TA1CTL |= MC_1;         //启动增计时
            TA1CCTL0 |= CCIE;       //使能定时器TA0CCR0.CCIFG中断
        }
        break;

        case E_TIMER_CHANNEL_A2:
        {
            TA2CCR0 = temp*125-1;   //设置定时器TA2计数值
            TA2CTL |= MC_1;         //启动增计时
            TA2CCTL0 |= CCIE;       //使能定时器TA0CCR0.CCIFG中断
        }
        break;

        case E_TIMER_CHANNEL_A3:    
        {
            TA3CCR0 = 10 - 1;            //设置定时器TA3计数值
            TA3CTL |= MC_1;         //启动增计时
            TA3CCTL0 |= CCIE;       //使能定时器TA0CCR0.CCIFG中断
        }
        break;

        case E_TIMER_CHANNEL_B0:
        {
            TB0CCR0 = temp*8000;    //设置定时器TB0计数值
            TB0CTL |= MC_1;         //启动增计时
            TB0CCTL0 |= CCIE;       //使能定时器TA0CCR0.CCIFG中断
        }
        break;

        default:
            break;
    }
}

/*******************************************************************************
**函数名:void DrvTimerDisable(TE_TIMER_CHANNEL cha)
**
**功能:禁止某个定时器工作，并清除定时器中断标志
**
**作者：HJH
**
**日期：2016-04-18
**
**修改内容：
**
**作者：
**
**日期：
*******************************************************************************/
void DrvTimerDisable(TE_TIMER_CHANNEL cha)
{
   switch(cha)
  {
    case E_TIMER_CHANNEL_A0:
      {
        TA0CTL &= ~MC_1;    //停止增计时
        TA0CCTL0 &= ~CCIE;  //禁止定时器TA0CCR0.CCIFG中断
        TA0CCTL0 &= ~CCIFG; //清除定时器中断标志
      }
      break;

    case E_TIMER_CHANNEL_A1:
      {
        TA1CTL &= ~MC_1;//停止增计时
        TA1CCTL0 &= ~CCIE;//禁止定时器TA1CCR0.CCIFG中断
        TA1CCTL0 &= ~CCIFG;//清除定时器中断标志
      }
      break;

    case E_TIMER_CHANNEL_A2:
      {
        TA2CTL   &= ~MC_1;//停止增计时
        TA2CCTL0 &= ~CCIE;//禁止定时器TA2CCR0.CCIFG中断
        TA2CCTL0 &= ~CCIFG;//清除定时器中断标志
      }
      break;

    case E_TIMER_CHANNEL_A3:
      {
        TA3CTL   &= ~MC_1;//停止增计时
        TA3CCTL0 &= ~CCIE;//禁止定时器TA3CCR0.CCIFG中断
        TA3CCTL0 &= ~CCIFG;//清除定时器中断标志
      }
      break;

    case E_TIMER_CHANNEL_B0:
      {
        TB0CTL   &= ~MC_1;//停止增计时
        TB0CCTL0 &= ~CCIE;//禁止定时器TB0CCR0.CCIFG中断
        TB0CCTL0 &= ~CCIFG;//清除定时器中断标志
      }
      break;

    default:
      break;
  }
}
/*******************************************************************************
**函数名:__interrupt void TimerA0Isr(void)
**
**功能:定时Timer_A TA0中断服务函数，
**
**作者：HJH
**
**日期：2016-04-19
**
**修改内容：
**
**作者：
**
**日期：
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
**函数名:__interrupt void TimerA1Isr(void)
**
**功能:定时Timer_A TA1中断服务函数，
**
**作者：HJH
**
**日期：2016-04-19
**
**修改内容：
**
**作者：
**
**日期：
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
**函数名:__interrupt void TimerA2Isr(void)
**
**功能:定时Timer_A TA2中断服务函数，
**
**作者：HJH
**
**日期：2016-04-19
**
**修改内容：
**
**作者：
**
**日期：
*******************************************************************************/
#pragma vector = TIMER2_A0_VECTOR
__interrupt void TimerA2Isr(void)
{
  CtlTimeOnOneTick(MSECS_PER_TICK);

  LPM1_EXIT;
}
/*******************************************************************************
**函数名:__interrupt void TimerA3Isr(void)
**
**功能:定时Timer_A TA3中断服务函数，
**
**作者：HJH
**
**日期：2016-04-19
**
**修改内容：
**
**作者：
**
**日期：
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
**函数名:__interrupt void TimerB0Isr(void)
**
**功能:定时Timer_B TB0中断服务函数，
**
**作者：HJH
**
**日期：2016-04-19
**
**修改内容：
**
**作者：
**
**日期：
*******************************************************************************/
#pragma vector = TIMER0_B0_VECTOR
__interrupt void TimerB0Isr(void)
{
  DrvTimerDisable(E_TIMER_CHANNEL_B0);

#ifdef _DEBUG		
 LPM3_EXIT;
#endif  
}
