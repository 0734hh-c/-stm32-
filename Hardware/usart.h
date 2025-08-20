#ifndef __USART_H_
#define __USART_H_

#include "stm32f10x.h"

// 定义USART2接收缓冲区大小
#define USART2_RX_BUF_SIZE 64

// USART2接收缓冲区结构
typedef struct {
    uint8_t buffer[USART2_RX_BUF_SIZE];
    volatile uint16_t head;
    volatile uint16_t tail;
    volatile uint8_t overflow;
} USART2_RxBuffer;

void USART1_Init(uint32_t USART_BaudRate);
void USART2_Init(uint32_t USART_BaudRate);
void USART_SEND_DATAS(USART_TypeDef * USARTx, int8_t * Data,uint32_t length);
uint16_t USART2_Available(void);
uint8_t USART2_ReadByte(void);
void USART2_ClearBuffer(void);
uint16_t USART2_ReadData(uint8_t *buffer, uint16_t len);
uint16_t USART2_PeekData(uint8_t *buffer, uint16_t len);

extern USART2_RxBuffer usart2_rx_buffer;

#endif
