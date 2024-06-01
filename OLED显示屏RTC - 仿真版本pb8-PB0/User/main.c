#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "oled.h"
#include "key16.h"
#include "LED.h"
#include "OLED.h"
#include "rtc.h"
#include "Buzzer.h"
#include "Timer.h"
#include "dht11.h"
#include "Clock.h"
#include "Usart.h"


#define size 16
#define date 5
#define time 28
#define week 45
extern u8 rmon,rday,rhour,rmin,rsec,rweek; //2位月日时分秒周
extern uint16_t Cyear[4],Cmon[4],Cday[4],Chour[4],Cmin[4],Csec[4];
extern u16 ryear; //4位年
extern uint16_t ClockTime[4];
extern u16 clock[4];
extern uint16_t Clock[20];
extern uint8_t testflag;
uint8_t KeyNum;
uint8_t MenuSelect;//菜单选择位
uint8_t MenuShow;//菜单显示标志位
uint8_t inputmode = 0;//输入模式标志位,默认普通模式
uint8_t mode;//菜单模式
uint8_t menuclear;//菜单清屏
uint8_t weekflag = 0;//是否显示周
uint8_t ClockSelect = 0,SetSelect = 0;
uint8_t s[4];
uint8_t RxData;
uint8_t data[30]={0};
uint8_t DHT_Buffer[5];//温湿度信息
uint8_t Sday,Shour,Smin,Ssec;//睡眠时间
uint16_t Sleep,Sh,Sm; 
uint8_t Sleepflag=0; 
uint8_t opentest = 1;//温湿度监测默认开启
uint8_t sayflag = 0;//报时标志位，默认不报时
uint8_t VOL = 20;//初始音量
u8 VOL_Cmd_Appoint[] = {0xAA, 0x13, 0x01, 0x00, 0x00};



uint8_t transflag (uint8_t flag){
	if (flag==1){
		flag = 0;
	}
	else{
		flag = 1;
	}
	return flag;
}
//以下为菜单选项
void Menu1(void)//设置时间
{
	OLED_Clear();
	switch(inputmode){
			case 0:
				TimeInput();break;
			case 1:
				QTimeInput();break;
			default :
				TimeInput();break;
		}
	
	OLED_Clear();
}
void Menu2(void)//闹钟设置
{
	OLED_Clear();
	while(1)
	{
		KeyNum = KEY_GET();
		switch(ClockSelect)
		{	
		case 0:
			s[0]=0;s[1]=1;s[2]=1;s[3]=1;break;
		case 1:
			s[0]=1;s[1]=0;s[2]=1;s[3]=1;break;
		case 2:
			s[0]=1;s[1]=1;s[2]=0;s[3]=1;break;
		case 3:
			s[0]=1;s[1]=1;s[2]=1;s[3]=0;break;
		}
	OLED_ShowString(0,0,"clock1:",16,s[0]);
	OLED_ShowNum(4*size,0,Chour[0],2,16,s[0]);
	OLED_ShowString(5*size,0,":",16,s[0]);		
	OLED_ShowNum(88,0,Cmin[0],2,16,s[0]);
		
	OLED_ShowString(0,1*size,"clock2:",16,s[1]);
	OLED_ShowNum(4*size,1*size,Chour[1],2,16,s[1]);
	OLED_ShowString(5*size,1*size,":",16,s[1]);		
	OLED_ShowNum(88,1*size,Cmin[1],2,16,s[1]);
		
	OLED_ShowString(0,2*size,"clock3:",16,s[2]);
	OLED_ShowNum(4*size,2*size,Chour[2],2,16,s[2]);
	OLED_ShowString(5*size,2*size,":",16,s[2]);		
	OLED_ShowNum(88,2*size,Cmin[2],2,16,s[2]);
		
	OLED_ShowString(0,3*size,"clock4:",16,s[3]);
	OLED_ShowNum(4*size,3*size,Chour[3],2,16,s[3]);
	OLED_ShowString(5*size,3*size,":",16,s[3]);		
	OLED_ShowNum(88,3*size,Cmin[3],2,16,s[3]);
	OLED_Refresh();
	if(KeyNum==14)  
		{
			ClockSelect++; //选择下一项
			ClockSelect%=4;
			OLED_Clear();
		}
	if(KeyNum==0)  
		{
			OLED_Clear();
			                                                                                                           
			switch(ClockSelect)
			{
			case 0:
				SClockInput(0);OLED_Clear();break;
			case 1:
				SClockInput(1);OLED_Clear();break;
			case 2:
				SClockInput(2);OLED_Clear();break;
			case 3:
				SClockInput(3);OLED_Clear();break;

			}
		}
		
		if(KeyNum==16)//暂时改成16
		{
			ClockSelect = 0;
			OLED_Clear();
			break;
		}	
	}
}

void Menu3(void)//查看温湿度
{
	
	OLED_Clear();
	OLED_ShowChinese(2*size,size,12,16,1);
		OLED_ShowChinese(3*size,size,14,16,1);//温度
		OLED_Refresh();
	while(1){
		
			OLED_ShowChinese(2*size,size,12,16,1);
			OLED_ShowChinese(3*size,size,14,16,1);//温度
			OLED_ShowString(4.2*size,size*1.3,":",8,1);
			
			OLED_ShowNum(5*size,size,DHT_Buffer[2],3,16,1);
			
			OLED_ShowChinese(6.5*size,size,38,16,1);
			
			OLED_ShowChinese(2*size,size*2,13,16,1);
			OLED_ShowChinese(3*size,size*2,14,16,1);//湿度
			OLED_ShowString(4.2*size,size*2.4,":",8,1);
			
			OLED_ShowNum(5*size,size*2,DHT_Buffer[0],3,16,1);
			OLED_ShowString(6.8*size,size*2,"%",16,1);
			OLED_Refresh();
		
		OLED_ShowChinese(2*size,size,12,16,1);
		OLED_ShowChinese(3*size,size,14,16,1);//温度
		OLED_Refresh();
		KeyNum = KEY_GET();
		if(KeyNum==16){
			OLED_Clear();
			break;
		}
		if(KeyNum==0){
			
			Delay_ms(1000);
			OLED_Clear();
		}
		
			
	}
}
void Menu4(void)//时间同步
{

	OLED_Clear();
	OLED_ShowChinese(24,24,17,16,0);//
	OLED_ShowChinese(24+1*size,24,18,16,0);//
	OLED_ShowChinese(24+2*size,24,35,16,0);//
	OLED_ShowChinese(24+3*size,24,36,16,0);//
	OLED_ShowChinese(24+4*size,24,37,16,0);//
	OLED_Refresh();
	uint8_t i = 0,j = 0;
	uint8_t m = 0;
	uint8_t decdata = 0;
	uint16_t nyear;
	uint8_t nmon,nday,nhour,nmin,nsec;
	Usart3_SendByte(99);
		while(i<7){
			if (Usart3_GetRxFlag() == 1)
			{
				RxData = Usart3_GetRxData();
				decdata = RxData%16+RxData/16*10;
				data[i] = decdata;
				m+=decdata;
				i++;
			}
			else{
				Usart3_SendByte(99);//补发消息
			}
			if (KEY_GET() == 0){
				break;
			}
		}
		nyear = data[0]*100+data[1];
		nmon = data[2];
		nday = data[3];
		nhour = data[4];
		nmin = data[5];
		nsec = data[6]+1;
		RTC_Set(nyear,nmon,nday,nhour,nmin,nsec);
		//同步时间后同步闹钟
		for(j = 0; j<4; j++){
		if(Cyear[j]!=0){//该位置有闹钟，需要再次设置计时时间
			Alarm_Set(j,Cyear[j],Cmon[j],Cday[j],Chour[j],Cmin[j],Csec[j]);
		}
	}
		OLED_Clear();
}

void Menu5(void)//睡眠时间记录
{
	Sday = rday;
	Shour = rhour;
	Smin = rmin;
	Ssec = rsec;
	Sleepflag = 1;
	OLED_DisPlay_Off();//关屏 = 睡觉
}

void Menu6(void)//设置
{
	
	OLED_Clear();
	while(1)
	{
		KeyNum = KEY_GET();
		switch(SetSelect)
		{	
		case 0:
			s[0]=0;s[1]=1;s[2]=1;s[3]=1;break;
		case 1:
			s[0]=1;s[1]=0;s[2]=1;s[3]=1;break;
		case 2:
			s[0]=1;s[1]=1;s[2]=0;s[3]=1;break;
		case 3:
			s[0]=1;s[1]=1;s[2]=1;s[3]=0;break;
		}
		OLED_ShowString(0,0,"mode:",16,s[0]);
		switch(inputmode)
		{
			case 0:
				OLED_ShowChinese(40,0,45,16,s[0]);//普通
				OLED_ShowChinese(40+1*size,0,46,16,s[0]);//
				OLED_Refresh();break;
			case 1:
				OLED_ShowChinese(40,0,39,16,s[0]);//快速
				OLED_ShowChinese(40+1*size,0,40,16,s[0]);//
				OLED_Refresh();break;
			default:
				OLED_ShowChinese(40,0,39,16,s[0]);//普通
				OLED_ShowChinese(40+1*size,0,40,16,s[0]);//
				OLED_Refresh();break;
		}
		OLED_ShowString(0,1*size,"voice:",16,s[1]);
		OLED_ShowNum(108,1*size,VOL,2,16,s[1]);
		OLED_Refresh();
		OLED_ShowString(0,2*size,"detection:",16,s[2]);
		switch(opentest)
		{
			case 0:
				OLED_ShowString(104,2*size,"OFF",16,s[2]);
				OLED_Refresh();break;
			case 1:
				OLED_ShowString(108,2*size,"ON",16,s[2]);
				OLED_Refresh();break;
			default:
				OLED_ShowString(108,2*size,"ON",16,s[2]);
				OLED_Refresh();break;
		}
		OLED_ShowString(0,3*size,"timekeeper:",16,s[3]);
		switch(sayflag)
		{
			case 0:
				OLED_ShowString(104,3*size,"OFF",16,s[3]);
				OLED_Refresh();break;
			case 1:
				OLED_ShowString(108,3*size,"ON",16,s[3]);
				OLED_Refresh();break;
			default:
				OLED_ShowString(104,3*size,"OFF",16,s[3]);
				OLED_Refresh();break;
		}
		
		OLED_Refresh();
	if(KeyNum==14)  
		{
			SetSelect++; //选择下一项
			SetSelect%=4;
			OLED_Clear();
		}
	if(SetSelect==1&&(KeyNum==2||KeyNum==8)){
		if(KeyNum==2&&VOL<30){
			VOL++;
			
			OLED_Refresh();
		}
		else if(KeyNum==8&&VOL>0){
			VOL--;
			
            OLED_Refresh();
		}
		USART_VOL(VOL_Cmd_Appoint,VOL);
	}
	if(KeyNum==0)  
		{
			OLED_Clear();
			                                                                                                           
			switch(SetSelect)
			{
			case 0:
				inputmode = transflag(inputmode);
				OLED_Clear();break;
			case 1:
				OLED_Clear();break;
			case 2:
				if(opentest==1){
					testflag = 0;
					opentest = 0;
					TIM_Cmd(TIM3,DISABLE);
				}
				else{
					testflag = 1;
					opentest = 1;
					TIM_Cmd(TIM3,ENABLE);
				}
				
				OLED_Clear();break;
			case 3:
				sayflag = transflag(sayflag);
				OLED_Clear();break;

			}
		}
		
		if(KeyNum==16)
		{
			SetSelect = 0;
			OLED_Clear();
			break;
		}	
	}
}
void Menu(void)//菜单
{	
	while(!MenuShow)
	{
		OLED_ShowChinese(48,0,20,16,1);
		OLED_ShowChinese(64,0,21,16,1);
		KeyNum=KEY_GET();
		if(KeyNum==14)  
		{	
			MenuSelect++; //选择下一项
			MenuSelect%=6;
			OLED_Clear();
		}
		if(KeyNum==16|KeyNum==13)  
		{	
			
			OLED_Clear();
			break;
		}
		switch(MenuSelect)
		{
		case 0:if(MenuSelect==0){
								OLED_ShowString(0,1*size,"1.",16,0);OLED_ShowChinese(1*size,1*size,22,16,0);OLED_ShowChinese(2*size,1*size,23,16,0);OLED_ShowChinese(3*size,1*size,24,16,0);OLED_ShowChinese(4*size,1*size,25,16,0);//设置时间
								OLED_ShowString(0,2*size,"2.",16,1);OLED_ShowChinese(1*size,2*size,22,16,1);OLED_ShowChinese(2*size,2*size,23,16,1);OLED_ShowChinese(3*size,2*size,26,16,1);OLED_ShowChinese(4*size,2*size,27,16,1);//设置闹钟
								OLED_ShowString(0,3*size,"3.",16,1);OLED_ShowChinese(1*size,3*size,28,16,1);OLED_ShowChinese(2*size,3*size,29,16,1);OLED_ShowChinese(3*size,3*size,12,16,1);OLED_ShowChinese(4*size,3*size,13,16,1);OLED_ShowChinese(5*size,3*size,14,16,1);//查看温湿度
								}
		case 1:if(MenuSelect==1){
								OLED_ShowString(0,1*size,"1.",16,1);OLED_ShowChinese(1*size,1*size,22,16,1);OLED_ShowChinese(2*size,1*size,23,16,1);OLED_ShowChinese(3*size,1*size,24,16,1);OLED_ShowChinese(4*size,1*size,25,16,1);//设置时间
								OLED_ShowString(0,2*size,"2.",16,0);OLED_ShowChinese(1*size,2*size,22,16,0);OLED_ShowChinese(2*size,2*size,23,16,0);OLED_ShowChinese(3*size,2*size,26,16,0);OLED_ShowChinese(4*size,2*size,27,16,0);//设置闹钟
								OLED_ShowString(0,3*size,"3.",16,1);OLED_ShowChinese(1*size,3*size,28,16,1);OLED_ShowChinese(2*size,3*size,29,16,1);OLED_ShowChinese(3*size,3*size,12,16,1);OLED_ShowChinese(4*size,3*size,13,16,1);OLED_ShowChinese(5*size,3*size,14,16,1);//查看温湿度
								}
				
		case 2:if(MenuSelect==2){
								OLED_ShowString(0,1*size,"1.",16,1);OLED_ShowChinese(1*size,1*size,22,16,1);OLED_ShowChinese(2*size,1*size,23,16,1);OLED_ShowChinese(3*size,1*size,24,16,1);OLED_ShowChinese(4*size,1*size,25,16,1);//设置时间
								OLED_ShowString(0,2*size,"2.",16,1);OLED_ShowChinese(1*size,2*size,22,16,1);OLED_ShowChinese(2*size,2*size,23,16,1);OLED_ShowChinese(3*size,2*size,26,16,1);OLED_ShowChinese(4*size,2*size,27,16,1);//设置闹钟
								OLED_ShowString(0,3*size,"3.",16,0);OLED_ShowChinese(1*size,3*size,28,16,0);OLED_ShowChinese(2*size,3*size,29,16,0);OLED_ShowChinese(3*size,3*size,12,16,0);OLED_ShowChinese(4*size,3*size,13,16,0);OLED_ShowChinese(5*size,3*size,14,16,0);//查看温湿度
								}
				
		case 3:if(MenuSelect==3){
								OLED_ShowString(0,1*size,"4.",16,0);OLED_ShowChinese(1*size,1*size,35,16,0);OLED_ShowChinese(2*size,1*size,36,16,0);OLED_ShowChinese(3*size,1*size,17,16,0);OLED_ShowChinese(4*size,1*size,18,16,0);//设置时间
								OLED_ShowString(0,2*size,"5.",16,1);OLED_ShowChinese(1*size,2*size,52,16,1);OLED_ShowChinese(2*size,2*size,53,16,1);OLED_ShowChinese(3*size,2*size,49,16,1);OLED_ShowChinese(4*size,2*size,54,16,1);//设置闹钟
								OLED_ShowString(0,3*size,"6.",16,1);OLED_ShowChinese(1*size,3*size,55,16,1);OLED_ShowChinese(2*size,3*size,56,16,1);//设置								}
								}
		case 4:if(MenuSelect==4){
								OLED_ShowString(0,1*size,"4.",16,1);OLED_ShowChinese(1*size,1*size,35,16,1);OLED_ShowChinese(2*size,1*size,36,16,1);OLED_ShowChinese(3*size,1*size,17,16,1);OLED_ShowChinese(4*size,1*size,18,16,1);//设置时间
								OLED_ShowString(0,2*size,"5.",16,0);OLED_ShowChinese(1*size,2*size,52,16,0);OLED_ShowChinese(2*size,2*size,53,16,0);OLED_ShowChinese(3*size,2*size,49,16,0);OLED_ShowChinese(4*size,2*size,54,16,0);//设置闹钟
								OLED_ShowString(0,3*size,"6.",16,1);OLED_ShowChinese(1*size,3*size,55,16,1);OLED_ShowChinese(2*size,3*size,56,16,1);//设置								}
								}
		case 5:if(MenuSelect==5){
								OLED_ShowString(0,1*size,"4.",16,1);OLED_ShowChinese(1*size,1*size,35,16,1);OLED_ShowChinese(2*size,1*size,36,16,1);OLED_ShowChinese(3*size,1*size,17,16,1);OLED_ShowChinese(4*size,1*size,18,16,1);//设置时间
								OLED_ShowString(0,2*size,"5.",16,1);OLED_ShowChinese(1*size,2*size,52,16,1);OLED_ShowChinese(2*size,2*size,53,16,1);OLED_ShowChinese(3*size,2*size,49,16,1);OLED_ShowChinese(4*size,2*size,54,16,1);//设置闹钟
								OLED_ShowString(0,3*size,"6.",16,0);OLED_ShowChinese(1*size,3*size,55,16,0);OLED_ShowChinese(2*size,3*size,56,16,0);//设置
								}
				OLED_Refresh();break;//查看温湿度			
		}
		if(KeyNum==0)//确定
		{	
			switch(MenuSelect)
			{
				case 0:mode=1;break;
				case 1:mode=2;break;
				case 2:mode=3;break;
				case 3:mode=4;break;
				case 4:mode=5;break;
				case 5:mode=6;break;
				default :break;
			}
			switch(mode)
			{
				case 1:Menu1();MenuShow=1;break;
				case 2:Menu2();MenuShow=1;break;
				case 3:Menu3();MenuShow=1;break;
				case 4:Menu4();MenuShow=1;break;
				case 5:Menu5();MenuShow=1;break;
				case 6:Menu6();MenuShow=1;break;
				default :break;
			}
			
		}
		
		
	}
				
				
		
}
int main(void)
{
	
	OLED_Init(); //屏幕
	OLED_ColorTurn(0);//0正常显示，1 反色显示
	OLED_DisplayTurn(0);//0正常显示 1 屏幕翻转显示
	key16_init();
	RTC_Init(); //RTC时钟配置
	Usart_Init();//串口通信
	
	Buzzer_Init(); //蜂鸣器
	Timer_Init(); //TIM定时器
	USART_VOL(VOL_Cmd_Appoint,VOL);//初始音量
 
	
	RTC_Set(2023,03,18,19,20,00);
	while(1)
	{   
		
		if(RTC_Get()==0)
		{
			OLED_ShowNum(10,date,ryear/1000,1,16,1);
			OLED_ShowNum(18,date,ryear%1000/100,1,16,1);
			OLED_ShowNum(26,date,ryear%100/10,1,16,1);
			OLED_ShowNum(34,date,ryear%10,1,16,1);
			OLED_ShowChinese(42,date,0,16,1);//年
			
			OLED_ShowNum(58,date,rmon%100/10,1,16,1);
			OLED_ShowNum(66,date,rmon%10,1,16,1);
			OLED_ShowChinese(74,date,1,16,1);//月
			
			//汉字+16，数字加8
			OLED_ShowNum(90,date,rday%100/10,1,16,1);
			OLED_ShowNum(98,date,rday%10,1,16,1);
			OLED_ShowChinese(106,date,2,16,1);//日
			
			OLED_ShowNum(34,time,rhour%100/10,1,16,1);
			OLED_ShowNum(42,time,rhour%10,1,16,1);
			OLED_ShowString(50,time+5,":",8,1);
			
			OLED_ShowNum(58,time,rmin%100/10,1,16,1);
			OLED_ShowNum(66,time,rmin%10,1,16,1);
			OLED_ShowString(74,time+5,":",8,1);
			
			OLED_ShowNum(82,time,rsec%100/10,1,16,1);
			OLED_ShowNum(90,time,rsec%10,1,16,1);
			
		}
		KeyNum = KEY_GET();
		if (KeyNum==13)//剩余时间，调试用
		{
			OLED_ShowNum(34,week,clock[0]-ClockTime[0],5,16,0);
			OLED_Refresh();
			Delay_ms(1000);
			OLED_Clear();	
		}
		if (KeyNum==14)//查看周
		{
			weekflag = transflag(weekflag);
			switch(weekflag)
			{
				case 0:
					OLED_Clear();break;
				case 1:
					OLED_ShowChinese(50-6,week,3,16,1);//星
					OLED_ShowChinese(66-6,week,4,16,1);//期
					switch (rweek)
					{
						case 1: OLED_ShowChinese(76,week,5,16,1);  break;//一
						case 2: OLED_ShowChinese(76,week,6,16,1);  break;
						case 3: OLED_ShowChinese(76,week,7,16,1);  break;
						case 4: OLED_ShowChinese(76,week,8,16,1);  break;
						case 5: OLED_ShowChinese(76,week,9,16,1);  break;
						case 6: OLED_ShowChinese(76,week,10,16,1);  break;
						case 0: OLED_ShowChinese(76,week,11,16,1);  break;
						default:OLED_ShowNum(76,week,rweek,1,16,1);
					}
					OLED_Refresh();break;
				default :
					OLED_Clear();break;
			}
		}
		
		if (KeyNum == 14)
		{
			if(Sleepflag==1){
				OLED_DisPlay_On();//亮屏 = 睡醒
				OLED_Clear();
				Sleep = Alarm_GetTime(ryear,rmon,rday,rhour,rmin,rsec)-Alarm_GetTime(ryear,rmon,Sday,Shour,Smin,Ssec);//睡觉时长（秒）
				Sh=Sleep/3600;
				Sm=Sleep%3600/60;
				OLED_ShowChinese(2*size,size,47,16,1);//本次休息：
				OLED_ShowChinese(3*size,size,48,16,1);
				OLED_ShowChinese(4*size,size,49,16,1);
				OLED_ShowChinese(5*size,size,50,16,1);
				
				OLED_ShowNum(2*size,2*size,Sh,2,16,1);
				OLED_ShowNum(4*size,2*size,Sm,2,16,1);
				OLED_ShowChinese(3*size,2*size,17,16,1);
				OLED_ShowChinese(5*size,2*size,51,16,1);
				OLED_Refresh();
				Sday = 0;
				Shour = 0;
				Smin = 0;
				Ssec = 0;//睡觉时间清零
				Delay_ms(3000);
				OLED_Clear();
				
				Sleepflag=0;
			}

		}
		if (KeyNum == 16)
		{
			OLED_Clear();
			MenuShow = 0;
			MenuSelect = 0;
			Menu();//进入菜单界面
		}
//		OLED_ShowNum(10,week,ClockTime[0],3,16,1);//显示TIM时钟，调试用

		OLED_Refresh();
		
		
		//闹钟
		if(clock[0]==ClockTime[0]&&(clock[0]!=0))
		{
			ClockResponse(0);
		}
		if(clock[1]==ClockTime[1]&&(clock[1]!=0))
		{
			ClockResponse(1);
		}
		if(clock[2]==ClockTime[2]&&(clock[2]!=0))
		{
			ClockResponse(2);
		}
		if(clock[3]==ClockTime[3]&&(clock[3]!=0))
		{
			ClockResponse(3);
		}
		//睡眠提醒
		if( rhour==22 && rmin==00&&rsec==00)
		{
			SleepTime();
		}
		if(sayflag==1&&rmin==00&&rsec==00)
		{
			ClockResponse(5);
		}
		if(testflag == 1){
			
			DHT_Get_Temp_Humi_Data(DHT_Buffer);
			if(DHT_Buffer[2]>40){
				OLED_Clear();
				OLED_ShowString(32,32,"temp!!",16,1);
				OLED_Refresh();
				Delay_ms(1000);
				OLED_Clear();
			}
			if(DHT_Buffer[0]>60){
				OLED_Clear();
				OLED_ShowString(32,32,"humi!!",16,1);
				OLED_Refresh();
				Delay_ms(1000);
				OLED_Clear();
			}
			
			
			testflag = 0;
		}
		
		
	}
	

}
