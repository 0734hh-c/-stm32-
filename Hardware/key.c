#include "stm32f10x.h"
#include "key.h"
#include "led.h"
/*
	初始化按键对应的GPIO--输入 无上下拉
	KEY0 -- PA0
	KEY1 -- PE2
	KEY2 -- PE3
	KEY3 -- PE4
*/
void KEY_Init(void)
{
	//1.使能时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOA, ENABLE);
	//2.设置为输入 默认(复位)情况下就是为输入 但是我们在这里需要显式声明
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_1 ;
	
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
}

/*
	读取按键:
	@key_num
	返回值:
		按键的状态
		按下
		松开
*/
uint8_t KEY_Status(KEY_NUM key_num)
{
    switch(key_num)
    {
        case KEY1://PE2  返回GPIOE_IDR 第2位
            return GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11);
        case KEY2://PE2  返回GPIOE_IDR 第2位
            return GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0);
        case KEY3://PE2  返回GPIOE_IDR 第2位
            return GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1);
        default:
            return KEY_NUM_ERROR;
    }
}



