#ifndef __FLAME_SENSOR_H
#define __FLAME_SENSOR_H

#include "stm32f10x.h"

// 火焰传感器状态
typedef enum {
    FLAME_NOT_DETECTED = 0,  // 未检测到火焰
    FLAME_DETECTED = 1       // 检测到火焰
} FlameState;

// 初始化火焰传感器
void FlameSensor_Init(void);

// 读取火焰传感器状态
FlameState FlameSensor_Read(void);

// 获取火焰检测计数
uint32_t FlameSensor_GetDetectionCount(void);

void FlameSensor_Waring(void);

#endif /* __FLAME_SENSOR_H */
