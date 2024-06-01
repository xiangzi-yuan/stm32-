#ifndef _KEY16_H
#define _KEY16_H
#include "stm32f10x.h"

#include <string.h>


void key16_init(void);
uint8_t KEY_SCAN(void);
uint8_t KEY_ROW_SCAN(void);
void HW_KEY_FUNCTION(void);
uint8_t KEY_GET(void);

#define KEY_CLO0_OUT_LOW  GPIO_WriteBit(GPIOB,GPIO_Pin_0,Bit_RESET)
#define KEY_CLO1_OUT_LOW  GPIO_WriteBit(GPIOA,GPIO_Pin_7,Bit_RESET)
#define KEY_CLO2_OUT_LOW  GPIO_WriteBit(GPIOA,GPIO_Pin_6,Bit_RESET)
#define KEY_CLO3_OUT_LOW  GPIO_WriteBit(GPIOA,GPIO_Pin_5,Bit_RESET)

#define KEY_CLO0_OUT_HIGH  GPIO_WriteBit(GPIOB,GPIO_Pin_0,Bit_SET) 
#define KEY_CLO1_OUT_HIGH  GPIO_WriteBit(GPIOA,GPIO_Pin_7,Bit_SET)
#define KEY_CLO2_OUT_HIGH  GPIO_WriteBit(GPIOA,GPIO_Pin_6,Bit_SET)
#define KEY_CLO3_OUT_HIGH  GPIO_WriteBit(GPIOA,GPIO_Pin_5,Bit_SET)


#endif

