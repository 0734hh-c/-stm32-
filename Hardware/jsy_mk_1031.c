#include "jsy_mk_1031.h"
#include "usart.h"
#include "delay.h"
#include <string.h>

// MODBUS CRC16计算
static uint16_t Modbus_CRC16(const uint8_t *data, uint16_t len) {
    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < len; i++) {
        crc ^= (uint16_t)data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }
    return crc;
}

// 读取JSY-MK-1031数据（使用中断接收）
uint8_t JSY_ReadData(USART_TypeDef *USARTx, uint8_t dev_addr, JSY_Data *data) {
    uint8_t tx_buf[8];
    uint8_t rx_buf[64] = {0};
    uint16_t crc;
    
    // 构建查询帧（功能码0x03）
    tx_buf[0] = dev_addr;                   // 设备地址
    tx_buf[1] = 0x03;                       // 功能码
    tx_buf[2] = (JSY_START_REG >> 8) & 0xFF;    // 起始地址高字节
    tx_buf[3] = JSY_START_REG & 0xFF;           // 起始地址低字节
    tx_buf[4] = (JSY_REG_COUNT >> 8) & 0xFF;    // 寄存器数量高字节
    tx_buf[5] = JSY_REG_COUNT & 0xFF;           // 寄存器数量低字节
    
    // 计算CRC并添加到帧
    crc = Modbus_CRC16(tx_buf, 6);
    tx_buf[6] = crc & 0xFF;                 // CRC低字节
    tx_buf[7] = (crc >> 8) & 0xFF;          // CRC高字节
    
    // 清除接收缓冲区
    USART2_ClearBuffer();
    
    // 发送请求
    USART_SEND_DATAS(USARTx, (int8_t*)tx_buf, 8);
    
    // 等待响应（带超时）
    uint32_t timeout = 200000; // 200ms超时(基于1us延时)
    uint16_t rx_len = 0;
    const uint16_t expected_len = 5 + 2 * JSY_REG_COUNT; // 期望长度35字节
    
    while(timeout--) {
        // 检查是否收到完整帧
        rx_len = USART2_Available();
        
        // 如果收到足够数据
        if(rx_len >= expected_len) {
            break;
        }
        
        // 如果收到部分数据但超时即将结束，检查帧完整性
        if(rx_len >= 5 && timeout < 50000) {
            // 窥视缓冲区检查帧长度
            uint8_t peek_buf[6];
            USART2_PeekData(peek_buf, 6);
            
            // 检查设备地址和功能码
            if(peek_buf[0] == dev_addr && peek_buf[1] == 0x03) {
                // 计算期望帧长度 = 地址(1)+功能码(1)+字节数(1)+数据(2*15)+CRC(2)
                uint8_t expected = 5 + peek_buf[2];
                if(rx_len >= expected) {
                    break;
                }
            }
        }
        
        delay_us(1);
    }
    
    // 检查超时
    if(timeout == 0) {
        return JSY_ERR_TIMEOUT;
    }
    
    // 读取接收到的数据
    rx_len = USART2_ReadData(rx_buf, sizeof(rx_buf));
    
    // 基础校验
    if(rx_len < 5) {
        return JSY_ERR_RESP_LEN;
    }
    if(rx_buf[0] != dev_addr) {
        return JSY_ERR_ADDR;
    }
    if(rx_buf[1] != 0x03) {
        return JSY_ERR_FUNC_CODE;
    }
    
    // 检查响应长度
    uint8_t byte_count = rx_buf[2];
    if(rx_len != (3 + byte_count + 2)) {
        return JSY_ERR_RESP_LEN;
    }
    
    // 检查字节数是否匹配寄存器数
    if(byte_count != 2 * JSY_REG_COUNT) {
        return JSY_ERR_BYTE_COUNT;
    }
    
    // CRC校验
    uint16_t recv_crc = (rx_buf[rx_len-1] << 8) | rx_buf[rx_len-2]; // Modbus CRC小端序
    if(Modbus_CRC16(rx_buf, rx_len - 2) != recv_crc) {
        return JSY_ERR_CRC;
    }
    
    // 解析数据（根据说明书寄存器映射）
    // 寄存器偏移说明：每个寄存器2字节，数据段从索引3开始
    
    // 电压 (0048H) - 寄存器0
    uint16_t volt = (rx_buf[3] << 8) | rx_buf[4];
    data->voltage = volt / 100.0f;
    
    // 电流 (0049H) - 寄存器1
    uint16_t current = (rx_buf[5] << 8) | rx_buf[6];
    data->current = current / 10000.0f;
    
    // 有功功率 (004CH) - 寄存器4
    uint16_t power = (rx_buf[11] << 8) | rx_buf[12];
    data->active_power = power / 10000.0f;
    
    // 电能 (004EH) - 寄存器6
    uint16_t energy = (rx_buf[15] << 8) | rx_buf[16];
    data->energy = energy / 100.0f;
    
    // 功率因数 (004FH) - 寄存器7
    uint16_t pf = (rx_buf[17] << 8) | rx_buf[18];
    data->power_factor = pf / 1000.0f;
    
    // 频率 (0050H) - 寄存器8
    uint16_t freq = (rx_buf[19] << 8) | rx_buf[20];
    data->frequency = freq / 100.0f;
    
    return JSY_OK;
}
