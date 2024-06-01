#ifndef __Usart_H
#define __Usart_H

#include <stdio.h>

void Usart_Init(void);
void Usart1_SendByte(uint8_t Byte);
void Usart1_SendArray(uint8_t *Array, uint16_t Length);
void Usart3_SendByte(uint8_t Byte);
void Usart3_SendArray(uint8_t *Array, uint16_t Length);
void Usart3_SendString(char *String);
void Usart3_SendNumber(uint32_t Number, uint8_t Length);
void Usart_Printf(char *format, ...);
void USART_MUSIC(u8 *mdate, u8 num);
void USART_VOL(u8 *mdate, u8 num);
uint8_t Usart1_GetRxFlag(void);
uint8_t Usart1_GetRxData(void);
uint8_t Usart3_GetRxFlag(void);
uint8_t Usart3_GetRxData(void);

#endif
