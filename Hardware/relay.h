#ifndef RELAY_H
#define RELAY_H

#include "stm32f10x.h"

// 继电器初始化
void Relay_Init(void);

// 打开继电器
void Relay_On(void);

// 关闭继电器
void Relay_Off(void);

// 切换继电器状态
void Relay_Toggle(void);

// 获取继电器状态
uint8_t Relay_GetState(void);

#endif // RELAY_H

