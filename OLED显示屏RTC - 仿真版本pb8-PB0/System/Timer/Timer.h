#ifndef __TIMER_H
#define __TIMER_H

void Timer_Init(void);

void Alarm_Response(void);
void Alarm_Set(uint8_t i,uint16_t year, uint16_t mon, uint16_t day, uint16_t hour, uint16_t min, uint16_t sec);
u16 Alarm_GetTime(u16 syear,u8 smon,u8 sday,u8 hour,u8 min,u8 sec);
void SleepTime (void);
void TimeInput(void);
void QTimeInput(void);

#endif
