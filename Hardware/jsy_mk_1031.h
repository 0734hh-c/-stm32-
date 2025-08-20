#ifndef JSY_MK_1031_H
#define JSY_MK_1031_H

#include "stm32f10x.h"
#include "usart.h"

// Modbus寄存器定义（根据说明书）
#define JSY_START_REG       0x0048  // 起始寄存器地址
#define JSY_REG_COUNT       0x000F  // 读取寄存器数量(15个)

// 错误代码
#define JSY_OK              0
#define JSY_ERR_TIMEOUT     1
#define JSY_ERR_CRC         2
#define JSY_ERR_FUNC_CODE   3
#define JSY_ERR_RESP_LEN    4
#define JSY_ERR_ADDR        5
#define JSY_ERR_BYTE_COUNT  6

// 数据结构
typedef struct {
    float voltage;       // 电压 (V)
    float current;       // 电流 (A)
    float active_power;  // 有功功率 (W)
    float energy;        // 电能 (kWh)
    float power_factor;  // 功率因数
    float frequency;     // 频率 (Hz)
} JSY_Data;

// 函数声明
uint8_t JSY_ReadData(USART_TypeDef *USARTx, uint8_t dev_addr, JSY_Data *data);

#endif // JSY_MK_1031_H
