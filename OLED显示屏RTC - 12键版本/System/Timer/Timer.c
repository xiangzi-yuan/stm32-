#include "stm32f10x.h"                  // Device header
#include "rtc.h" 
#include "Buzzer.h" 
#include "OLED.h"
#include "Delay.h"
#include "Key16.h"
#include "Clock.h"

extern uint16_t Cyear[4],Cmon[4],Cday[4],Chour[4],Cmin[4],Csec[4];
extern u8 rmon,rday,rhour,rmin,rsec,rweek; //2位月日时分秒周
extern u16 ryear; //4位年
extern u8 table_week[12];
extern u8 mon_table[12];
uint16_t ClockTime[4]; //TIM计时器，当与clock[]值相同时触发闹钟
u16 clock[4] = {0};//最终计时时间 
uint16_t TIME[20]={0};
uint8_t testflag = 0;
#define size 16
#define NumSize 8


void Timer_Init(void)
{
	//第一步，RCC 开启时钟,这里注意,要使用 APB1的开启时钟函数,因为TIM2是 APB1总线的外设
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	//第二步，选择时基单元的时钟源(因为定时器上电后默认就是使用内部时钟,可以不写)
	
	TIM_InternalClockConfig(TIM2);
	TIM_InternalClockConfig(TIM3);
	//第三步，配置时基单元,包括这里的预分频器Prescaler、自动重装器Period、计数模式CounterMode等等
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision= TIM_CKD_DIV1;//滤波采样的分频数，作用不大
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;//分别是向上计数、向下计数和三种中央对齐的模式
	TIM_TimeBaseInitStructure.TIM_Period = 10000 - 1; //0~65535 ,加到这个值重装一下
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7200 - 1;//CK_CNT_OV = CK_PSC / (PSC + 1) / (ARR + 1) = 1Hz , 1s

	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;//高级寄存器才有
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseInitStructure);
	
	TIM_TimeBaseInitStructure.TIM_Period = 40000 - 1; //0~65535 ,加到这个值重装一下
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7200 - 1;//0.25Hz,4s
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);
	
	//第四步，配置输出中断控制，允许更新中断输出到 NVIC
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE); //使能更新中断
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //使能更新中断
	
	//取消初始中断,避免初始化完就进入中断
	TIM_ClearFlag(TIM2,TIM_FLAG_Update);
	TIM_ClearFlag(TIM3,TIM_FLAG_Update);
	//第五步， 配置 NVIC， 在 NVIC 中打开定时器中断的通道， 并分配一个优先级
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//优先级分组
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; //中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority  = 2;
	
	NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;//很重要！
	NVIC_InitStructure.NVIC_IRQChannelSubPriority  = 3;
	NVIC_Init(&NVIC_InitStructure);
	//第六步，就是运行控制了
	//整个模块配置完成后，我们还需要使能一下计数器
	TIM_Cmd(TIM2,ENABLE);
	TIM_Cmd(TIM3,ENABLE);
}

u16 Alarm_GetTime(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec)
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
	return seccount;	    
}
//最后再写一个定时器的中断函数
//模板
void TIM2_IRQHandler (void)
{
	if(TIM_GetITStatus(TIM2,TIM_IT_Update)==SET)
	{
		ClockTime[0]++;
		ClockTime[1]++;
		ClockTime[2]++;
		ClockTime[3]++;
		TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
	}
	
}
void TIM3_IRQHandler (void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET)
	{
		
		if(testflag==0){
			testflag = 1;
		}
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
	}
	
}

void Alarm_Set(uint8_t i,uint16_t year, uint16_t mon, uint16_t day, uint16_t hour, uint16_t min, uint16_t sec)
{
	
	clock[i] = Alarm_GetTime(year,mon,day,hour,min,sec)-Alarm_GetTime(ryear,rmon,rday,rhour,rmin,rsec);
	ClockTime[i] = 0;//ClockTime重新计时
}

void TimeInput(void)
{
	uint16_t time,i=0,j=0;
	uint16_t year;
	uint8_t mon,day,hour,min,sec;
	while(i!=14){
			OLED_ShowChinese(1*size,0*size,30,16,1);
			OLED_ShowChinese(2*size,0*size,31,16,1);
			OLED_ShowChinese(3*size,0*size,32,16,1);
			OLED_ShowChinese(4*size,0*size,24,16,1);
			OLED_ShowChinese(5*size,0*size,25,16,1);//请输入时间
			OLED_ShowString(6*size,0,":",16,1);
			OLED_Refresh();
			time = KEY_GET();
			if(time==16){
				if(i==0)
				{
					break;
				}
				else
				{
					OLED_ShowString((i-1)*NumSize,2*size,"_",16,1);
					i--;//点s13键重新输入
				}
				
			}
			if(time!=99&&time<13){//屏蔽错误输入
				OLED_ShowNum(i*NumSize,2*size,time,1,16,1);
				TIME[i] = time;
				i++;
			} 
			if(time==14){
				i=0;//点s16键取消输入
				OLED_Clear();
				break;
			}
	
	}
	year = TIME[0]*1000+TIME[1]*100+TIME[2]*10+TIME[3];
	mon = TIME[4]*10+TIME[5];
	day = TIME[6]*10+TIME[7];
	hour = TIME[8]*10+TIME[9];
	min = TIME[10]*10+TIME[11];
	sec = TIME[12]*10+TIME[13];
	RTC_Set(year,mon,day,hour,min,sec);//重新写入时间
	//以下重新计算闹钟
	for(j = 0; j<4; j++){
		if(Cyear[j]!=0){//该位置有闹钟，需要再次设置计时时间
			Alarm_Set(j,Cyear[j],Cmon[j],Cday[j],Chour[j],Cmin[j],Csec[j]);
		}
	}
}

void QTimeInput(void)
{
	uint16_t time,i=0,j=0;
	uint16_t year;
	uint8_t mon,day,hour,min,sec;
	while(i<4){
			OLED_ShowChinese(1*size,0*size,30,16,1);
			OLED_ShowChinese(2*size,0*size,31,16,1);
			OLED_ShowChinese(3*size,0*size,32,16,1);
			OLED_ShowChinese(4*size,0*size,24,16,1);
			OLED_ShowChinese(5*size,0*size,25,16,1);//请输入时间
			OLED_ShowString(6*size,0,":",16,1);
			OLED_ShowString(60,2*size,":",16,1);
			OLED_Refresh();
			time = KEY_GET();
			if(time==16){
				if(i==0)
				{
					break;
				}
				else if(i<3){//仅为了退格准确优美，无实际作用
					OLED_ShowString((i-1)*NumSize+40,2*size,"_",16,1);
					i--;
				}
				else{
					OLED_ShowString((i-1)*NumSize+56,2*size,"_",16,1);
					i--;
				}

			}
			if(time!=99&&time<13){//屏蔽错误输入
				if(i<2){//条件分支仅为了输入优美
					OLED_ShowNum(i*NumSize+40,2*size,time,1,16,1);
				}
				else{
					OLED_ShowNum(i*NumSize+56,2*size,time,1,16,1);
				}
				TIME[i] = time;
				i++;
			}
			if(time==14){
				i=0;//点s16键取消输入
				OLED_Clear();
				break;
			}
	
	}
	year = ryear;
	mon = rmon;
	day = rday;
	hour = TIME[0]*10+TIME[1];
	min = TIME[2]*10+TIME[3];
	sec = 0;

	
	RTC_Set(year,mon,day,hour,min,sec);//只写入时分,秒清零
	//以下重新计算闹钟
	for(j = 0; j<4; j++){
		if(Cyear[j]!=0){//该位置有闹钟，需要再次设置计时时间
			Alarm_Set(j,Cyear[j],Cmon[j],Cday[j],Chour[j],Cmin[j],Csec[j]);
		}
	}
}
