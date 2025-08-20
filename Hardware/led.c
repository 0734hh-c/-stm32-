#include "stm32f10x.h"
#include "led.h"
/*
	LED_Init:初始化LED相关的GPIO口
	@void
	返回值:void
*/


void LED_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable,ENABLE);
	//使能时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
	//配置GPIO口
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_15; 
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3; 
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	GPIO_SetBits(GPIOA, GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_15);
	GPIO_SetBits(GPIOB, GPIO_Pin_3);
}

/*
	LED_Ctl:控制led灯
	@led_num:控制哪盏灯
	@led_status:灯的状态
	返回值：void
*/
void LED_Ctl(LED_NUM led_num,LED_STATUS led_status)
{

	switch(led_num)
	{
		case D0:
			GPIO_WriteBit(GPIOB,GPIO_Pin_3,(BitAction)led_status);break;
		case D1:

			GPIO_WriteBit(GPIOA,GPIO_Pin_15,(BitAction)led_status);break;
		case D2:
			GPIO_WriteBit(GPIOA,GPIO_Pin_12,(BitAction)led_status);break;
		case D3:
			GPIO_WriteBit(GPIOA,GPIO_Pin_11,(BitAction)led_status);break;
	}
}


void LED_SET(void)
{
	if (GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_11) == 0)		//获取输出寄存器的状态，如果当前引脚输出低电平
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_11);					//则设置PA1引脚为高电平
	}
	else													//否则，即当前引脚输出高电平
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_11);					//则设置PA1引脚为低电平
	}
	
	if (GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_12) == 0)		//获取输出寄存器的状态，如果当前引脚输出低电平
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_12);					//则设置PA1引脚为高电平
	}
	else													//否则，即当前引脚输出高电平
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_12);					//则设置PA1引脚为低电平
	}
	
	if (GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_15) == 0)		//获取输出寄存器的状态，如果当前引脚输出低电平
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_15);					//则设置PA1引脚为高电平
	}
	else													//否则，即当前引脚输出高电平
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_15);					//则设置PA1引脚为低电平
	}

	if (GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_3) == 0)		//获取输出寄存器的状态，如果当前引脚输出低电平
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_3);					//则设置PA1引脚为高电平
	}
	else													//否则，即当前引脚输出高电平
	{
		GPIO_ResetBits(GPIOB, GPIO_Pin_3);					//则设置PA1引脚为低电平
	}
}


