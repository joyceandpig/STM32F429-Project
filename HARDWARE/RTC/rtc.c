#include "rtc.h"
#include "led.h"
#include "delay.h"
#include "usart.h" 
#include "calendar.h"
#include "stm32f4xx_hal_rtc.h"
#include "stm32f4xx_hal_rtc_ex.h"
#include "stm32f4xx.h"

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32开发板
//RTC 驱动代码	    
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/12/27
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved		
//********************************************************************************
//V1.1 20160706
//针对综合实验，进行了修改，防止死循环						  
////////////////////////////////////////////////////////////////////////////////// 	 

RTC_HandleTypeDef RTC_Handler;
RTC_TimeTypeDef RTC_TimeStruct;
RTC_DateTypeDef RTC_DateStruct;



//等待RSF同步
//返回值:0,成功;1,失败;
u8 RTC_Wait_Synchro(void)
{ 
	u32 retry=0XFFFFF; 
	//关闭RTC寄存器写保护
	RTC->WPR=0xCA;
	RTC->WPR=0x53; 
	RTC->ISR&=~(1<<5);		//清除RSF位 
	while(retry&&((RTC->ISR&(1<<5))==0x00))//等待影子寄存器同步
	{
		retry--;
	}
    if(retry==0)return 1;	//同步失败 
	RTC->WPR=0xFF;			//使能RTC寄存器写保护  
	return 0;
}
//BCD码转换为十进制数据
//val:要转换的BCD码
//返回值:十进制数据
u8 RTC_BCD2DEC(u8 val)
{
	u8 temp=0;
	temp=(val>>4)*10;
	return (temp+(val&0X0F));
}
//获取RTC时间
//*hour,*min,*sec:小时,分钟,秒钟 
//*ampm:AM/PM,0=AM/24H,1=PM.
void RTC_Get_Time(u8 *hour,u8 *min,u8 *sec,u8 *ampm)
{
	u8 retry=0;
	u32 temp=0;
 	while(RTC_Wait_Synchro()){retry++;if(retry>10)break;};//等待同步  	 
	temp=RTC->TR;
	*hour=RTC_BCD2DEC((temp>>16)&0X3F);
	*min=RTC_BCD2DEC((temp>>8)&0X7F);
	*sec=RTC_BCD2DEC(temp&0X7F);
	*ampm=temp>>22; 
	
//	__HAL_RTC_WRITEPROTECTION_DISABLE(&RTC_Handler);
//  HAL_RTC_WaitForSynchro(&RTC_Handler);
//  __HAL_RTC_WRITEPROTECTION_ENABLE(&RTC_Handler);
//	
//	HAL_RTC_GetTime(&RTC_Handler,&RTC_TimeStruct,RTC_FORMAT_BIN);
//	printf("sec:%d\r\n",RTC_TimeStruct.Seconds);
//	*hour = RTC_TimeStruct.Hours;
//	*min = RTC_TimeStruct.Minutes;
//	*sec = RTC_TimeStruct.Seconds;
//	*ampm = RTC_TimeStruct.TimeFormat;
}
//获取RTC日期
//*year,*mon,*date:年,月,日
//*week:星期
void RTC_Get_Date(u8 *year,u8 *month,u8 *date,u8 *week)
{
	__HAL_RTC_WRITEPROTECTION_DISABLE(&RTC_Handler);
  HAL_RTC_WaitForSynchro(&RTC_Handler);
  __HAL_RTC_WRITEPROTECTION_ENABLE(&RTC_Handler);
	
	HAL_RTC_GetDate(&RTC_Handler,&RTC_DateStruct,RTC_FORMAT_BIN);
	*year = RTC_DateStruct.Year;
	*month = RTC_DateStruct.Month;
	*date = RTC_DateStruct.Date;
	*week = RTC_DateStruct.WeekDay;
}
//RTC时间设置
//hour,min,sec:小时,分钟,秒钟
//ampm:@RTC_AM_PM_Definitions:RTC_HOURFORMAT12_AM/RTC_HOURFORMAT12_PM
//返回值:SUCEE(1),成功
//       ERROR(0),进入初始化模式失败 
HAL_StatusTypeDef RTC_Set_Time(u8 hour,u8 min,u8 sec,u8 ampm)
{
	RTC_TimeTypeDef RTC_TimeStructure;

	RTC_TimeStructure.Hours=hour;
	RTC_TimeStructure.Minutes=min;
	RTC_TimeStructure.Seconds=sec;
	RTC_TimeStructure.TimeFormat=ampm;
	RTC_TimeStructure.DayLightSaving=RTC_DAYLIGHTSAVING_NONE;
  RTC_TimeStructure.StoreOperation=RTC_STOREOPERATION_RESET;
	return HAL_RTC_SetTime(&RTC_Handler,&RTC_TimeStructure,RTC_FORMAT_BIN);	
}

//RTC日期设置
//year,month,date:年(0~99),月(1~12),日(0~31)
//week:星期(1~7,0,非法!)
//返回值:SUCEE(1),成功
//       ERROR(0),进入初始化模式失败 
HAL_StatusTypeDef RTC_Set_Date(u8 year,u8 month,u8 date,u8 week)
{
	RTC_DateTypeDef RTC_DateStructure;
	
	RTC_DateStructure.Date=date;
	RTC_DateStructure.Month=month;
	RTC_DateStructure.WeekDay=week;
	RTC_DateStructure.Year=year;
	return HAL_RTC_SetDate(&RTC_Handler,&RTC_DateStructure,RTC_FORMAT_BIN);
}

//RTC初始化
//返回值:0,初始化成功;
//       2,进入初始化模式失败;
u8 RTC_Init(void)
{      
		RTC_Handler.Instance=RTC;
    RTC_Handler.Init.HourFormat=RTC_HOURFORMAT_24;//RTC设置为24小时格式 
    RTC_Handler.Init.AsynchPrediv=0X7F;           //RTC异步分频系数(1~0X7F)
    RTC_Handler.Init.SynchPrediv=0XFF;            //RTC同步分频系数(0~7FFF)   
    RTC_Handler.Init.OutPut=RTC_OUTPUT_DISABLE;     
    RTC_Handler.Init.OutPutPolarity=RTC_OUTPUT_POLARITY_HIGH;
    RTC_Handler.Init.OutPutType=RTC_OUTPUT_TYPE_OPENDRAIN;
    if(HAL_RTC_Init(&RTC_Handler)!=HAL_OK) return 2;
      
    if(HAL_RTCEx_BKUPRead(&RTC_Handler,RTC_BKP_DR0)!=0X5050)//是否第一次配置
    { 
        RTC_Set_Time(23,59,46,RTC_HOURFORMAT12_PM);	        //设置时间 ,根据实际时间修改
				RTC_Set_Date(15,12,27,7);		                    //设置日期
        HAL_RTCEx_BKUPWrite(&RTC_Handler,RTC_BKP_DR0,0X5050);//标记已经初始化过了
    }
		
		RTC_Set_WakeUp(RTC_WAKEUPCLOCK_CK_SPRE_16BITS,0); //配置WAKE UP中断,1秒钟中断一次  
    return 0;
}

//RTC底层驱动，时钟配置
//此函数会被HAL_RTC_Init()调用
//hrtc:RTC句柄
void HAL_RTC_MspInit(RTC_HandleTypeDef* hrtc)
{
    RCC_OscInitTypeDef RCC_OscInitStruct;
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct;

    __HAL_RCC_PWR_CLK_ENABLE();//使能电源时钟PWR
		HAL_PWR_EnableBkUpAccess();//取消备份区域写保护
    
    RCC_OscInitStruct.OscillatorType=RCC_OSCILLATORTYPE_LSE;//LSE配置
    RCC_OscInitStruct.PLL.PLLState=RCC_PLL_NONE;
    RCC_OscInitStruct.LSEState=RCC_LSE_ON;                  //RTC使用LSE
    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    PeriphClkInitStruct.PeriphClockSelection=RCC_PERIPHCLK_RTC;//外设为RTC
    PeriphClkInitStruct.RTCClockSelection=RCC_RTCCLKSOURCE_LSE;//RTC时钟源为LSE
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct);
        
    __HAL_RCC_RTC_ENABLE();//RTC时钟使能
}

//设置闹钟时间(按星期闹铃,24小时制)
//week:星期几(1~7) @ref  RTC_WeekDay_Definitions
//hour,min,sec:小时,分钟,秒钟
void RTC_Set_AlarmA(u8 week,u8 hour,u8 min,u8 sec)
{ 
    RTC_AlarmTypeDef RTC_AlarmSturuct;
    
    RTC_AlarmSturuct.AlarmTime.Hours=hour;  //小时
    RTC_AlarmSturuct.AlarmTime.Minutes=min; //分钟
    RTC_AlarmSturuct.AlarmTime.Seconds=sec; //秒
    RTC_AlarmSturuct.AlarmTime.SubSeconds=0;
    RTC_AlarmSturuct.AlarmTime.TimeFormat=RTC_HOURFORMAT12_AM;
    
    RTC_AlarmSturuct.AlarmMask=RTC_ALARMMASK_NONE;//精确匹配星期，时分秒
    RTC_AlarmSturuct.AlarmSubSecondMask=RTC_ALARMSUBSECONDMASK_NONE;
    RTC_AlarmSturuct.AlarmDateWeekDaySel=RTC_ALARMDATEWEEKDAYSEL_WEEKDAY;//按星期
    RTC_AlarmSturuct.AlarmDateWeekDay=week; //星期
    RTC_AlarmSturuct.Alarm=RTC_ALARM_A;     //闹钟A
    HAL_RTC_SetAlarm_IT(&RTC_Handler,&RTC_AlarmSturuct,RTC_FORMAT_BIN);
    
    HAL_NVIC_SetPriority(RTC_Alarm_IRQn,0x01,0x02); //抢占优先级1,子优先级2
    HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
}

//周期性唤醒定时器设置  
/*wksel:  @ref RTCEx_Wakeup_Timer_Definitions
#define RTC_WAKEUPCLOCK_RTCCLK_DIV16        ((uint32_t)0x00000000)
#define RTC_WAKEUPCLOCK_RTCCLK_DIV8         ((uint32_t)0x00000001)
#define RTC_WAKEUPCLOCK_RTCCLK_DIV4         ((uint32_t)0x00000002)
#define RTC_WAKEUPCLOCK_RTCCLK_DIV2         ((uint32_t)0x00000003)
#define RTC_WAKEUPCLOCK_CK_SPRE_16BITS      ((uint32_t)0x00000004)
#define RTC_WAKEUPCLOCK_CK_SPRE_17BITS      ((uint32_t)0x00000006)
*/
//cnt:自动重装载值.减到0,产生中断.
void RTC_Set_WakeUp(u32 wksel,u16 cnt)
{ 
    __HAL_RTC_WAKEUPTIMER_CLEAR_FLAG(&RTC_Handler, RTC_FLAG_WUTF);//清除RTC WAKE UP的标志
	
		HAL_RTCEx_SetWakeUpTimer_IT(&RTC_Handler,cnt,wksel);            //设置重装载值和时钟 
	
    HAL_NVIC_SetPriority(RTC_WKUP_IRQn,0x02,0x02); //抢占优先级1,子优先级2
    HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
}

//RTC闹钟中断服务函数
void RTC_Alarm_IRQHandler(void)
{
    HAL_RTC_AlarmIRQHandler(&RTC_Handler);
}
    
//RTC闹钟A中断处理回调函数
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
    printf("ALARM A!\r\n");
}

//RTC WAKE UP中断服务函数
void RTC_WKUP_IRQHandler(void)
{
    HAL_RTCEx_WakeUpTimerIRQHandler(&RTC_Handler);  
}

//RTC WAKE UP中断处理
void HAL_RTCEx_WakeUpTimerEventCallback(RTC_HandleTypeDef *hrtc)
{
//    LED1=!LED1;
}

u8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //月修正数据表	  
//获得现在是星期几
//功能描述:输入公历日期得到星期(只允许1901-2099年)
//year,month,day：公历年月日 
//返回值：星期号(1~7,代表周1~周日)																						 
u8 RTC_Get_Week(u16 year,u8 month,u8 day)
{	
	u16 temp2;
	u8 yearH,yearL;
	yearH=year/100;	yearL=year%100; 
	// 如果为21世纪,年份数加100  
	if (yearH>19)yearL+=100;
	// 所过闰年数只算1900年之后的  
	temp2=yearL+yearL/4;
	temp2=temp2%7; 
	temp2=temp2+day+table_week[month-1];
	if (yearL%4==0&&month<3)temp2--;
	temp2%=7;
	if(temp2==0)temp2=7;
	return temp2;
}	













