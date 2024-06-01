#include "stm32f10x.h"                  // Device header
#include "rtc.h" 
#include "Buzzer.h" 
#include "OLED.h"
#include "Delay.h"
#include "Key16.h"
#include "Timer.h"
#include "bmp.h"
#include "Usart.h"
#define size 16
#define NumSize 8
extern uint8_t inputmode;
extern u8 rmon,rday,rhour,rmin,rsec,rweek; //2位月日时分秒周
extern u16 ryear; //4位年
u8 Music_Cmd_Appoint[] = {0xAA,0x07,0x02,0x00,0x00,0x00};	//播放指定曲目初始化数组
uint16_t Clock[20]={0};//用于输入闹钟
uint16_t Cyear[8],Cmon[8],Cday[8],Chour[8],Cmin[8],Csec[8];//用于存储闹钟
//作为闹钟函数用来产生软中断
void Clock_Init(void)
{
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStructure;
 
	EXTI_InitStruct.EXTI_Line=EXTI_Line2;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising_Falling; //下降沿触发
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStruct); //初始化中断线参数
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn; //选择外部中断2
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x02; //抢占优先级 2，
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02; //子优先级 2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);
}

void SleepTime (void)
{
	OLED_Clear();
			
	OLED_ShowChinese(34,20,15,16,1);
	OLED_ShowChinese(50,20,16,16,1);//睡觉			
	OLED_ShowChinese(34,36,17,16,1);
	OLED_ShowChinese(50,36,18,16,1);
	
	OLED_Refresh();
	Delay_ms(2000);
	OLED_Clear();
}


void QClockInput(uint8_t Which)
{
	uint16_t time,i=0;
	while(i<4){
			OLED_ShowChinese(1*size,0*size,30,16,0);
			OLED_ShowChinese(2*size,0*size,31,16,0);
			OLED_ShowChinese(3*size,0*size,32,16,0);
			OLED_ShowChinese(4*size,0*size,33,16,0);
			OLED_ShowChinese(5*size,0*size,34,16,0);//请输入闹钟
			OLED_ShowString(6*size,0,":",16,1);
			OLED_ShowString(60,2*size,":",16,1);
			OLED_Refresh();
			time = KEY_GET();
			if(time==13){
				if(i==0)
				{
					break;
				}
				else if(i<3){
					OLED_ShowString((i-1)*NumSize+40,2*size,"_",16,1);
					i--;
				}
				else{
					OLED_ShowString((i-1)*NumSize+56,2*size,"_",16,1);
					i--;
				}

			}
			if(time!=99&&time<13){//屏蔽错误输入
				if(i<2){
					OLED_ShowNum(i*NumSize+40,2*size,time,1,16,1);
				}
				else{
					OLED_ShowNum(i*NumSize+56,2*size,time,1,16,1);
				}
				Clock[i] = time;
				i++;
			}
			if(time==16){
				i=0;//点s16键重新输入
				OLED_Clear();
			}
	
	}
	Cyear[Which]=ryear;
	Cmon[Which] =rmon;
	Cday[Which] =rday;
	Chour[Which] =Clock[0]*10+Clock[1];
	Cmin[Which] = Clock[2]*10+Clock[3];
	Csec[Which] = 0;
	
	Alarm_Set(Which,Cyear[Which],Cmon[Which],Cday[Which],Chour[Which],Cmin[Which],Csec[Which]);
}
void ClockInput(uint8_t Which)
{
	uint16_t time,i=0;
	while((KEY_GET()!=15)&&(i!=14)){//输14位自动确认，原因不明
			OLED_ShowChinese(1*size,0*size,30,16,0);
			OLED_ShowChinese(2*size,0*size,31,16,0);
			OLED_ShowChinese(3*size,0*size,32,16,0);
			OLED_ShowChinese(4*size,0*size,33,16,0);
			OLED_ShowChinese(5*size,0*size,34,16,0);//请输入闹钟
			OLED_ShowString(6*size,0,":",16,1);
			OLED_Refresh();
			time = KEY_GET();
			if(time==13){
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
				Clock[i] = time;
				i++;
				
			}
			if(time==16){
				i=0;//点s16键重新输入
				OLED_Clear();
			}
	
	}
	Cyear[Which]=Clock[0]*1000+Clock[1]*100+Clock[2]*10+Clock[3];
	Cmon[Which] =Clock[4]*10+Clock[5];
	Cday[Which] =Clock[6]*10+Clock[7];
	Chour[Which] =Clock[8]*10+Clock[9];
	Cmin[Which] = Clock[10]*10+Clock[11];
	Csec[Which] = Clock[12]*10+Clock[13];
	Alarm_Set(Which,Cyear[Which],Cmon[Which],Cday[Which],Chour[Which],Cmin[Which],Csec[Which]);
	
}
void SClockInput(uint8_t Which)
{
	switch(inputmode){
			case 0:
				ClockInput(Which);break;
			case 1:
				QClockInput(Which);break;
			default :
				ClockInput(Which);break;
		}
}
void PlayStata(void)
{
	uint8_t i,stata[] = {0xAA, 0x01, 0x00, 0xAB},tstata[10] = {0};
	uint8_t RxData,sum=0;
	Usart1_SendArray(stata,4);
	while(1){
	for (i = 0;i<9;i++){
		if (Usart1_GetRxFlag() == 1)
			{
				RxData = Usart1_GetRxData();
				tstata[i] = RxData;
				sum += tstata[i];
			}
		else{
			Usart1_SendArray(stata,4);
		}
		
		}
	if(tstata[5]==172){
		break;
		}
	}
	
	
}
uint8_t delay_ms(u16 time,u8 KEY)
{    
   u16 i=0,keyNum;
    if(KEY==13){
		return 13;
	}
   while(time--)
   {
	  keyNum = KEY_GET();
	   if(keyNum == 13) return keyNum;
      i=12000;  
      while(i--){
		  
		  if(keyNum == 13) return keyNum;

	  } 
   }
   return 0;
}
void ClockResponse(uint8_t Which)
{
	uint8_t i=0,KeyNum1,KeyNum2;
	uint8_t stop[]={0xAA, 0x04, 0x00, 0xAE};
	
	uint8_t play[] = {64,Chour[Which],61,Cmin[Which],62};
	if(Which==5){
		play[0] = 64;
		play[1] = rhour;
		play[2] = 65;
		play[3] = 0;
		play[4] = 0;
	}
	OLED_Clear();
	for(i=0;i<5;i++){
		OLED_ShowPicture(32,0,64,64,BMP1,1);
		OLED_Refresh();
		KeyNum1 = KEY_GET();
		if(play[i]){
			USART_MUSIC(Music_Cmd_Appoint, play[i]);
		}
		KeyNum2 = delay_ms(30,KeyNum1);
		
		if(KeyNum1==13 || KeyNum2==13){
			Usart1_SendArray(stop,4);
			break;
		}
		PlayStata();
		
	}
	OLED_Clear();
}

void EXTI2_IRQHandler(void)
{
	if(EXTI_GetITStatus( EXTI_Line2) != RESET)
	{
		Buzzer_ON();
		EXTI_ClearFlag(EXTI_Line2);    //清除中断标志位
		EXTI_ClearITPendingBit(EXTI_Line2); //清除EXTI线路挂起位
	}
}
