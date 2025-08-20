#include "stm32f10x.h"
#include "usart.h"
#include "stdio.h" 

// USART2接收缓冲区
USART2_RxBuffer usart2_rx_buffer = {{0}, 0, 0, 0};

// 串口1初始化
void USART1_Init(uint32_t USART_BaudRate)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    GPIO_InitTypeDef  GPIO_InitStruct = {0};
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;  // TX
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10; // RX
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    USART_InitTypeDef USART_InitStruct = {0};
    USART_InitStruct.USART_BaudRate = USART_BaudRate;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART1, &USART_InitStruct);
    
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    
    NVIC_InitTypeDef NVIC_InitStruct = {0};
    NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
    NVIC_Init(&NVIC_InitStruct);
    
    USART_Cmd(USART1, ENABLE);
}

// 串口2初始化（用于JSY模块）
void USART2_Init(uint32_t USART_BaudRate)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;  // TX
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3;  // RX
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    USART_InitTypeDef USART_InitStruct = {0};
    USART_InitStruct.USART_BaudRate = USART_BaudRate;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART2, &USART_InitStruct);
    
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    
    NVIC_InitTypeDef NVIC_InitStruct = {0};
    NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1; // 提高优先级
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStruct);
    
    // 初始化接收缓冲区
    usart2_rx_buffer.head = 0;
    usart2_rx_buffer.tail = 0;
    usart2_rx_buffer.overflow = 0;
    
    USART_Cmd(USART2, ENABLE);
}

// 发送数据
void USART_SEND_DATAS(USART_TypeDef * USARTx, int8_t * Data, uint32_t length)
{
    uint32_t i;
    for(i = 0; i < length; i++)
    {
        USART_SendData(USARTx, (uint8_t)Data[i]);
        while(USART_GetFlagStatus(USARTx, USART_FLAG_TXE) != SET);
    }
}

// 串口1中断服务函数
uint16_t USART1_Data;
void USART1_IRQHandler(void)
{
    if(USART_GetITStatus( USART1, USART_IT_RXNE) == SET)
    {
        USART1_Data = USART_ReceiveData(USART1);
        // 移除了回显功能
        USART_ClearITPendingBit(USART1, USART_IT_RXNE);
    }
}

// 串口2中断服务函数（用于JSY模块）
void USART2_IRQHandler(void)
{
    if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
    {
        uint8_t data = USART_ReceiveData(USART2);
        
        // 将数据存入环形缓冲区
        uint16_t next = (usart2_rx_buffer.head + 1) % USART2_RX_BUF_SIZE;
        if(next != usart2_rx_buffer.tail) {
            usart2_rx_buffer.buffer[usart2_rx_buffer.head] = data;
            usart2_rx_buffer.head = next;
        } else {
            usart2_rx_buffer.overflow = 1; // 缓冲区溢出
        }
        
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    }
}

// 获取USART2接收缓冲区可用数据量
uint16_t USART2_Available(void)
{
    if(usart2_rx_buffer.head >= usart2_rx_buffer.tail) {
        return usart2_rx_buffer.head - usart2_rx_buffer.tail;
    }
    return USART2_RX_BUF_SIZE - usart2_rx_buffer.tail + usart2_rx_buffer.head;
}

// 从USART2接收缓冲区读取一个字节
uint8_t USART2_ReadByte(void)
{
    if(usart2_rx_buffer.head == usart2_rx_buffer.tail) {
        return 0; // 缓冲区为空
    }
    
    uint8_t data = usart2_rx_buffer.buffer[usart2_rx_buffer.tail];
    usart2_rx_buffer.tail = (usart2_rx_buffer.tail + 1) % USART2_RX_BUF_SIZE;
    return data;
}

// 清除USART2接收缓冲区
void USART2_ClearBuffer(void)
{
    usart2_rx_buffer.head = 0;
    usart2_rx_buffer.tail = 0;
    usart2_rx_buffer.overflow = 0;
}

// 从USART2接收缓冲区读取多个字节
uint16_t USART2_ReadData(uint8_t *buffer, uint16_t len)
{
    uint16_t count = 0;
    while(USART2_Available() > 0 && count < len) {
        buffer[count++] = USART2_ReadByte();
    }
    return count;
}

// 窥视USART2接收缓冲区数据（不移除）
uint16_t USART2_PeekData(uint8_t *buffer, uint16_t len)
{
    uint16_t count = 0;
    uint16_t tail = usart2_rx_buffer.tail;
    
    while(tail != usart2_rx_buffer.head && count < len) {
        buffer[count++] = usart2_rx_buffer.buffer[tail];
        tail = (tail + 1) % USART2_RX_BUF_SIZE;
    }
    return count;
}

// 重定向printf
struct __FILE
{
    int handle;
};

FILE __stdout;

int fputc(int c, FILE* stream)
{
    USART_SendData(USART1, c & 0xff);
    while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) != SET);
    return 0;
}
