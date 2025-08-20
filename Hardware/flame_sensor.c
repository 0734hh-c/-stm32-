#include "flame_sensor.h"
#include "beep.h"
#include "delay.h"
#include "relay.h"

// 火焰检测计数器
static uint32_t flameDetectionCount = 0;

void FlameSensor_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 使能GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    // 配置PA1为输入模式
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  // 上拉输入模式
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 初始化计数器
    flameDetectionCount = 0;
}

FlameState FlameSensor_Read(void) {
    // 读取PA1引脚状态
    // 当检测到火焰时，传感器输出低电平
    if (GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1) == Bit_SET) {
        return FLAME_NOT_DETECTED;
    } else {
        flameDetectionCount++;
        return FLAME_DETECTED;
    }
}

uint32_t FlameSensor_GetDetectionCount(void) {
    return flameDetectionCount;
}

void FlameSensor_Waring(void) {
    FlameSensor_Read();
    if(FlameSensor_GetDetectionCount() > 10) {
        B1;  // 开启蜂鸣器
        delay_ms(10000);
        B0;  // 关闭蜂鸣器
        Relay_Off();
        flameDetectionCount = 0;
    }
}
