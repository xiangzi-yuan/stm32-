#include "stm32f10x.h"                  // Device header
#include "Delay.h"

void Buzzer_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_ResetBits(GPIOA, GPIO_Pin_8);
}

void Buzzer_ON(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_8);
}

void Buzzer_OFF(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_8);
}

void Buzzer_Turn(void)
{
	if (GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_8) == 0)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_8);
	}
	else
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_8);
	}
}
void Buzzer_1000ms(void)
{
	Buzzer_ON();
	Delay_ms(1000);
	Buzzer_OFF();
}
