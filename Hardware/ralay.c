#include "relay.h"

// 继电器控制引脚定义
#define RELAY_GPIO_PORT GPIOB
#define RELAY_GPIO_PIN  GPIO_Pin_3

// 继电器状态
static uint8_t relayState = 0; // 0:关闭, 1:开启

void Relay_Init(void) {
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable,ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // 使能GPIOC时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    
    // 配置PC13为推挽输出
    GPIO_InitStructure.GPIO_Pin = RELAY_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // 2MHz输出速度
    GPIO_Init(RELAY_GPIO_PORT, &GPIO_InitStructure);
    
    // 初始状态为关闭
    Relay_Off();
}

void Relay_On(void) {
    // 高电平触发
    GPIO_SetBits(RELAY_GPIO_PORT, RELAY_GPIO_PIN);
    relayState = 1;
}

void Relay_Off(void) {
    // 低电平关闭
    GPIO_ResetBits(RELAY_GPIO_PORT, RELAY_GPIO_PIN);
    relayState = 0;
}

void Relay_Toggle(void) {
    if (relayState) {
        Relay_Off();
    } else {
        Relay_On();
    }
}

uint8_t Relay_GetState(void) {
    return relayState;
}

