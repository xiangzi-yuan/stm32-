#ifndef __DHT11_H
#define __DHT11_H




uint8_t DHT_Get_Temp_Humi_Data(uint8_t buffer[]);
void DHT_GPIO_Init(GPIOMode_TypeDef Mode);


#endif
