#include "delay.h"

volatile uint32_t g_tickCount = 0;  // 全局计时变量

void delay_ms(uint16_t n)
{
	if(n==0) return ;
	uint16_t d,m,i,k=500;
	d = n/k;//有多少个k
	m = n%k;//还剩多少
	d = m?(d+1):d;
	for(i=0;i<d;i++)
	{
		//最后一次延时的时间
		if(i==d-1&&m)
		{
			k=m;
		}
		//1.选择时钟源 默认为外部时钟源168MHZ/8
		SysTick->CTRL &= ~(0X01<<2);
		//2.设置重装载值
		SysTick->LOAD = SystemCoreClock/8/1000*k-1;
		//3.开始计数
		SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;//0X01<<0;
	//	  /*#define SysTick_CTRL_ENABLE_Msk (1UL /*<< SysTick_CTRL_ENABLE_Pos*/)  
	//	/*!< SysTick CTRL: ENABLE Mask */
		//4.等待到计数为0的时候结束
		uint32_t tmp;
		do 
		{
			tmp = SysTick->CTRL;
		}
		while((tmp&0x01)&&!(tmp&(0x01<<16)));//等待溢出或者提前关闭了定时器
		//5.关闭定时器
		SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
		//6.清理CNT 如果提前关了定时器 则CNT里面有残值
		SysTick->VAL = 0x00;
		}
}

void delay_ms_lim(uint16_t n)
{


	//1.选择时钟源默认为外部时钟源168MHZ/8
	SysTick->CTRL&=~(0X01<<2);
	//2.设置重装载值
	SysTick->LOAD=SystemCoreClock/8/1000-1;//3.开始计数
	uint32_t tmp;
	while(n)
	{
		SysTick->CTRL&=~(0X01<<2);
		SysTick->CTRL|= SysTick_CTRL_ENABLE_Msk;
		do
		{
		tmp=SysTick->CTRL;
		}
		while((tmp&0x01)&&!(tmp&(0x01<<16)));
		SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;//6.清理CNT如果提前关了定时器则CNT里面有残值
		SysTick->VAL=0x00;
		n--;
	}
	
}
void delay_us(uint16_t n)
{
	if(n == 0) return;
	//1.选择时钟源默认为外部时钟源168MHZ/8
	SysTick->CTRL&=~(0X01<<2);
	//2.设置重装载值
	SysTick->LOAD=n*SystemCoreClock/8/1000000-1;//3.开始计数
	SysTick->CTRL|= SysTick_CTRL_ENABLE_Msk;
	uint32_t tmp;
	do
	{
	tmp=SysTick->CTRL;
	}
	while((tmp&0x01)&&!(tmp&(0x01<<16)));
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;//6.清理CNT如果提前关了定时器则CNT里面有残值
	SysTick->VAL=0x00;
}

// SysTick中断处理函数
void SysTick_Handler(void) {
    g_tickCount++;
}

// 获取系统时间（单位：ms）
uint32_t GetTick(void) {
    return g_tickCount;
}

// 初始化系统时钟（SysTick）
void Tick_Init(void) {
    // 配置SysTick为1ms中断
    SysTick->LOAD = SystemCoreClock / 8000 - 1;  // 72MHz / 8000 = 9kHz -> 1ms
    SysTick->VAL = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | 
                   SysTick_CTRL_TICKINT_Msk | 
                   SysTick_CTRL_ENABLE_Msk;
    
    // 设置中断优先级
    NVIC_SetPriority(SysTick_IRQn, 0);
}

