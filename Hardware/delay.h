#ifndef DELAY_H_
#define DELAY_H_

#include "stm32f10x.h"
#include <stdint.h>

void delay_ms(uint16_t n);

void delay_ms_lim(uint16_t n);

void delay_us(uint16_t n);

// 声明全局计时变量
extern volatile uint32_t g_tickCount;

// 获取系统时间（单位：ms）
uint32_t GetTick(void);

// 初始化系统时钟（SysTick）
void Tick_Init(void);


#endif
