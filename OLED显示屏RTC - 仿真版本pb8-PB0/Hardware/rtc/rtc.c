/* 
	//时间读写与设置说明//
1，在main函数开头放入RTC_Config();就可以使能时钟了。
在RTC_Config();函数中自带判断是不是首次使用RTC
2，使用 RTC_Get();读出时间。读出的数据存放在：
年 ryear （16位）
月 rmon	 （以下都是8位）
日 rday
时 rhour
分 rmin
秒 rsec
周 rweek

3，使用 RTC_Set(4位年,2位月,2位日,2位时,2位分,2位秒); 写入时间。例如：RTC_Get(2017,08,06,21,34,00);

其他函数都是帮助如上3个函数的，不需要调用。 
注意要使用RTC_Get和RTC_Set的返回值，为0时表示读写正确。

*/


#include "sys.h"
#include "rtc.h"
#include "Buzzer.h"
#include "Delay.h"
#include "OLED.h"

//以下2条全局变量--用于RTC时间的读取
u16 ryear; //4位年
u8 rmon,rday,rhour,rmin,rsec,rweek;//当前时间
extern uint16_t Cyear[4],Cmon[4],Cday[4],Chour[4],Cmin[4],Csec[4];//用于存储闹钟


//只用全局中断
static void RTC_NVIC_Config(void)
{	
  NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;		
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		
	NVIC_Init(&NVIC_InitStructure);		
}

u8 RTC_Init(void)
{

    u8 temp=0;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	
	PWR_BackupAccessCmd(ENABLE);	
	if (BKP_ReadBackupRegister(BKP_DR1) != 0x5051)		
		{	 			 
			BKP_DeInit(); //复位备份区域
		RCC_LSEConfig(RCC_LSE_ON);	//开启LSE时钟
//		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET&&temp<250)	
//			{
//			temp++;
//			Delay_ms(10);
//			}
//		if(temp>=250)return 1;	    
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);		  
		RCC_RTCCLKCmd(ENABLE);	
		RTC_WaitForLastTask();	
		RTC_WaitForSynchro();		 
		RTC_ITConfig(RTC_IT_SEC|RTC_IT_ALR, ENABLE);	
		RTC_WaitForLastTask();	
		RTC_EnterConfigMode();
		RTC_SetPrescaler(32767); 
		RTC_WaitForLastTask();	
//		RTC_Set(2023,2,01,21,59,30);  
        
		RTC_ExitConfigMode(); 
		BKP_WriteBackupRegister(BKP_DR1, 0X5051);	
		}
	else
		{

		RTC_WaitForSynchro();	
		RTC_ITConfig(RTC_IT_SEC, ENABLE);	
		RTC_WaitForLastTask();	
		}
	RTC_NVIC_Config();	    				     
	RTC_Get();	
	return 0; 

}		 	


//两个中断都用
//static void RTC_NVIC_Config(void)
//{ /*  闹钟中断的优先级必须必秒中断高
// 闹钟中断和秒中断几乎同时到来 秒中断的处理函数 是RTC_IRQHandler()
// 如果进入这个函数 那么要想从RTC_IRQHandler()退出  则必须清除所有中断标志
//    (包括闹钟中断)， 这样 闹钟中断标志被清除 则RTCAlarm_IRQHandler()函数肯定是进不去了
// 如果不清楚闹钟中断标志 那么程序会死在RTC_IRQHandler()里边
// 综上所述 那种中断必须能打断秒中断的执行 这样程序才能执行到RTCAlarm_IRQHandler()里边
//   */
//	 NVIC_InitTypeDef NVIC_InitStructure;
//	 NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;  //RTC全局中断
//	 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; 
//	 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1; 
//	 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  //使能该通道中断
//	 NVIC_Init(&NVIC_InitStructure);  

//	 NVIC_InitStructure.NVIC_IRQChannel = RTCAlarm_IRQn;  //闹钟中断
//	 NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; //比RTC全局中断的优先级高
//	 NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2; 
//	 NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
//	 NVIC_Init(&NVIC_InitStructure);
//}

//static void RTC_Alarm_EXIT(void)
//{
//	EXTI_InitTypeDef EXTI_InitStructure;
//	EXTI_ClearITPendingBit(EXTI_Line17);
//	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
//    EXTI_InitStructure.EXTI_Line = EXTI_Line17;
//    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
//	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
//    EXTI_Init(&EXTI_InitStructure);

//}


void RTC_IRQHandler(void)
{		 
	if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
	{							
		RTC_Get();  
			
 	}
	if(RTC_GetITStatus(RTC_IT_ALR)!= RESET)
	{
	//一般中断来说，建议只是在中断处理函数中，置一个中断标志位，简短易行。然后其他部分的执行，
	//是来扫描这个标志位做事，这样就会很好来避免这个问题。	
		
  	} 				  								 
	RTC_ClearITPendingBit(RTC_IT_SEC|RTC_IT_OW);
	RTC_WaitForLastTask();	  	    						 	   	 
}


//void RTCAlarm_IRQHandler(void)
//{     if(RTC_GetITStatus(RTC_IT_ALR) != RESET)
//      {
//		
//      }
//     EXTI_ClearITPendingBit(EXTI_Line17);
//  RTC_WaitForLastTask();
//  RTC_ClearITPendingBit(RTC_IT_ALR);
//  RTC_WaitForLastTask();
// }

/*如果你不需要使用RTC闹铃唤醒STOP模式的话，那就没必要使用RTCAlarm_IRQHandler()，只
使用RTC全局中断来处理闹铃就可以了。RTCAlarm_IRQHandler这个中断是为了唤醒STOP模式而
设计的，这个中断必须要同时使能EXTI_17的中断，这个时候才要考虑中断优先级的问题*/

//void RTCAlarm_IRQHandler(void){	//闹钟中断处理（启用时必须调高其优先级）
//	if(RTC_GetITStatus(RTC_IT_ALR) != RESET){
//	
//	}
//	RTC_ClearITPendingBit(RTC_IT_ALR);
//	RTC_WaitForLastTask();
//}

//判断是否是闰年函数
//月份   1  2  3  4  5  6  7  8  9  10 11 12
//闰年   31 29 31 30 31 30 31 31 30 31 30 31
//非闰年 31 28 31 30 31 30 31 31 30 31 30 31
//输入:年份
//输出:该年份是不是闰年.1,是  0,不是
u8 Is_Leap_Year(u16 year){                    
	if(year%4==0)
	{ //必须能被4整除
		if(year%100==0)
			{		
			if(year%400==0)
				return 1;//如果以00结尾,还要能被400整除          
			else 
				return 0;  
			}
			else 
				return 1;  
	}
	else 
		return 0;
}                           
//设置时钟
//把输入的时钟转换为秒钟
//以1970年1月1日为基准
//1970~2099年为合法年份

//月份数据表                                                                       
u8 const table_week[12]={0,3,3,6,1,4,6,2,5,0,3,5}; //月修正数据表  
const u8 mon_table[12]={31,28,31,30,31,30,31,31,30,31,30,31};//平年的月份日期表

//写入时间
u8 RTC_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec){ //写入当前时间（1970~2099年有效），
	u16 t;
	u32 seccount=0;
	if(syear<2000||syear>2099)return 1;//syear范围1970-2099，此处设置范围为2000-2099       
	for(t=1970;t<syear;t++){ //把所有年份的秒钟相加
		if(Is_Leap_Year(t))seccount+=31622400;//闰年的秒钟数
		else seccount+=31536000;                    //平年的秒钟数
	}//按365天算，也就是平年，有365*24*60*60=31536000秒； 
	 //按366天算，也就是闰年，有366*24*60*60=31622400秒；
	//以上算出完整年份时间，今年没有过完不算
	smon-=1;
	// 本月没有过完，例如三月，只有1,2月完整
	//smon = 2, t从0到1
	//t = 0,时间按表+1月时间
	//t = 1,时间按表+2月时间
	for(t=0;t<smon;t++){         //把前面月份的秒钟数相加
		seccount += (u32)mon_table[t]*86400;//月份秒钟数相加
		if(Is_Leap_Year(syear)&&t==1)seccount+=86400;//Is_Leap_Year(syear)=1,闰年2月份增加一天的秒钟数        
	}
	//计算本月时间30号就是29天*86400+hour*3600+min*60+sec
	seccount+=(u32)(sday-1)*86400;//把前面日期的秒钟数相加
	seccount+=(u32)hour*3600;//小时秒钟数
	seccount+=(u32)min*60;      //分钟秒钟数
	seccount+=sec;//最后的秒钟加上去
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	
	PWR_BackupAccessCmd(ENABLE);	// 
	RTC_SetCounter(seccount);	//
	RTC_WaitForLastTask();

	return 0; //返回值:0,成功;其他:错误代码.    
}

//读出时间
u8 RTC_Get(void){//读出当前时间值 //返回值:0,成功;其他:错误代码.
	static u16 daycnt=0;
	u32 timecount=0;
	u32 temp=0;
	u16 temp1=0;
	timecount=RTC_GetCounter();		
	temp=timecount/86400;   //得到天数(秒钟数对应的)
	if(daycnt!=temp){//超过一天了
		daycnt=temp;
		temp1=1970;  //从1970年开始
		while(temp>=365){
		     if(Is_Leap_Year(temp1)){//是闰年
			     if(temp>=366)temp-=366;//闰年的秒钟数
			     else {temp1++;break;} 
		     }
		     else temp-=365;       //平年
		     temp1++; //1970往后加
		}  
		ryear=temp1;//得到年份
		temp1=0;
		//除去年份剩余天数temp
		while(temp>=28){//超过了一个月(平年2月28天)
			if(Is_Leap_Year(ryear)&&temp1==1){//当年是闰年2月份
				if(temp>=29)temp-=29;//如果是闰年2月份且剩余天数>=29则减去29
				else break;			 //否则证明是闰年2月份且天数只有28天，跳出月份循环
			}else{//当年不是闰年2月份，天数不少于一个月
	            if(temp>=mon_table[temp1])temp-=mon_table[temp1];//平年
	            else break;//当年不是闰年2月份，天数少于一个月，跳出月份循环
			}
			temp1++; 
		}
		rmon=temp1+1;//得到月份
		rday=temp+1;  //得到日期
	}
	temp=timecount%86400;     //取余得到不足一天的秒钟数      
	rhour=temp/3600;     //小时
	rmin=(temp%3600)/60; //分钟     
	rsec=(temp%3600)%60; //秒钟
	rweek=RTC_Get_Week(ryear,rmon,rday);//获取星期  
	return 0;
}    

u8 RTC_Get_Week(u16 year,u8 month,u8 day){ //按年月日计算星期(只允许1901-2099年)//已由RTC_Get调用    
	u16 temp2;
	u8 yearH,yearL;
	yearH=year/100;     
	yearL=year%100;
	// 如果为21世纪,年份数加100 
	if (yearH>19)yearL+=100;
	// 所过闰年数只算1900年之后的 
	temp2=yearL+yearL/4;
	temp2=temp2%7;
	temp2=temp2+day+table_week[month-1];
	if (yearL%4==0&&month<3)temp2--;
	return(temp2%7); //返回星期值
}

//设置中断闹钟
u8 RTC_Alarm_Set(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec)
{
	u16 t;
	u32 seccount=0;
	if(syear<1970||syear>2099)return 1;	   
	for(t=1970;t<syear;t++)	
	{
		if(Is_Leap_Year(t))seccount+=31622400;
		else seccount+=31536000;			 
	}
	smon-=1;
	for(t=0;t<smon;t++)	   
	{
		seccount+=(u32)mon_table[t]*86400;
		if(Is_Leap_Year(syear)&&t==1)seccount+=86400;	   
	}
	seccount+=(u32)(sday-1)*86400;
	seccount+=(u32)hour*3600;
    seccount+=(u32)min*60;	 
	seccount+=sec;			    
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);	//启用PWR和BKP的时钟（from APB1）
	PWR_BackupAccessCmd(ENABLE);//后备域解锁	 
	RTC_SetAlarm(seccount);
	//当可编程计数器的值与RTC_ALR中的32位值相等时，即触发一个闹钟事件，并且产生RTC闹钟
	//中断。此寄存器受RTC_CR寄存器里的RTOFF位写保护，仅当RTOFF值为’1’时，允许写操作。
	RTC_WaitForLastTask();	
	
	return 0;	    
}
