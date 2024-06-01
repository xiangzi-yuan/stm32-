#include "key16.h"
#include "Delay.h"


uint8_t Key_row[1]={0xff};   //定义一个数组，存放行扫描状态

//端口的配置
void key16_init(){

	GPIO_InitTypeDef GPIO_InitStruture;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //打开PB时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); //打开PA时钟
	
	//定义PB0、PA7、PA6、PA5为推挽输出 
	GPIO_InitStruture.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruture.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStruture.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB,&GPIO_InitStruture);
	
	GPIO_InitStruture.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruture.GPIO_Pin = GPIO_Pin_7 | GPIO_Pin_6 | GPIO_Pin_5;
	GPIO_InitStruture.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruture);
	
	//定义PA8、PA9、PA10、PA11为上拉输入 分别定义为四行
	GPIO_InitStruture.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruture.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_1;
	GPIO_Init(GPIOA,&GPIO_InitStruture);
	GPIO_InitStruture.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruture.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6;
	GPIO_Init(GPIOB,&GPIO_InitStruture);
	

}

/***
 *函数名：KEY_ROW_SCAN
 *功  能：按键行扫描
 *返回值：1~4，对应1~4行按键位置
 */
//如果为1，代表没有按键被按下，如果为0，代表有按键被按下
uint8_t KEY_ROW_SCAN(void)
{
    //读出行扫描状态
    Key_row[0] = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)<<3;
    Key_row[0] = Key_row[0] | (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_5)<<2);
    Key_row[0] = Key_row[0] | (GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)<<1);
    Key_row[0] = Key_row[0] | (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_1));
    
	if(Key_row[0] != 0x0f)         //不是1111，代表肯定有一个0行
    {
      Delay_ms(10);                    //消抖
      if(Key_row[0] != 0x0f)
		  //0111 1011 1101 1110
        {   
                //printf("Key_Row_DATA = 0x%x\r\n",Key_row[0]);
                switch(Key_row[0])
                {
                    case 0x07:         //0111 判断为该列第1行的按键按下
                        return 1;
                    case 0x0b:         //1011 判断为该列第2行的按键按下
                        return 2;
                    case 0x0d:         //1101 判断为该列第3行的按键按下
                        return 3;
                    case 0x0e:         //1110 判断为该列第4行的按键按下
                        return 4;
                    default :
                        return 0;
                }
        }
        else return 0;
    }
    else return 0;
}

/***
 *函数名：KEY_SCAN
 *功  能：4*4按键扫描
 *返回值：0~16，对应16个按键
 */
uint8_t KEY_SCAN(void)
{    
    uint8_t Key_Num=99;            //1-16对应的按键数
    uint8_t key_row_num=0;        //行扫描结果记录

    KEY_CLO0_OUT_LOW;        
    if( (key_row_num=KEY_ROW_SCAN()) != 0 )
    { 
        while(KEY_ROW_SCAN() != 0);  //消抖
        Key_Num = 0 + key_row_num*10;
    }
    KEY_CLO0_OUT_HIGH;
    
    KEY_CLO1_OUT_LOW;        
    if( (key_row_num=KEY_ROW_SCAN()) != 0 )
    { 
        while(KEY_ROW_SCAN() != 0);
        Key_Num = (3 + key_row_num)*9;
    }
    KEY_CLO1_OUT_HIGH;
    
    KEY_CLO2_OUT_LOW;    
    if( (key_row_num=KEY_ROW_SCAN()) != 0 )
    { 
        while(KEY_ROW_SCAN() != 0);
    Key_Num = (6 + key_row_num)*8;
    }
    KEY_CLO2_OUT_HIGH;
    
    KEY_CLO3_OUT_LOW;    
    if( (key_row_num=KEY_ROW_SCAN()) != 0 )
    {
        while(KEY_ROW_SCAN() != 0);
        Key_Num =12 + key_row_num;
    }
    KEY_CLO3_OUT_HIGH;
	
    return Key_Num;
}
uint8_t KEY_GET(void)
{
	uint8_t K;
	K = KEY_SCAN();
	if(K == 80){
		K = 1;
	}
	if(K == 72){
		K = 4;
	}
	if(K == 64){
		K = 7;
	}
	if(K == 56){
		K = 14;//next
	}
	if(K == 63){
		K = 2;
	}
	if(K == 54){
		K = 5;
	}
	if(K == 45){
		K = 8;
	}
	if(K == 36){
		K = 0;//确定
	}
	if(K == 40){
		K = 3;
	}
	if(K == 30){
		K = 6;
	}
	if(K == 20){
		K = 9;
	}
	
	if(K == 10){
		K = 16;//菜单键，取消键
	}
	return K;
}
