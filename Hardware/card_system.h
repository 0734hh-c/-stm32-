#ifndef __CARD_SYSTEM_H
#define __CARD_SYSTEM_H

#include "stm32f10x.h"
#include "MFRC522.h"
#include "W25Q64.h"
#include "menu.h"

// 卡数据结构
typedef struct {
    uint8_t uid[4];     // 卡UID (4字节)
    uint32_t balance;    // 余额 (单位：分)
    uint8_t secret[6];   // 密码 (6字节)
} CardInfo;

// 卡系统初始化
void CardSystem_Init(void);

// 检测是否有卡
uint8_t CardSystem_DetectCard(void);

// 读取卡信息
uint8_t CardSystem_ReadCard(CardInfo *card);

// 保存卡信息
void CardSystem_SaveCard(CardInfo *card);

// 消费处理
uint8_t CardSystem_Charge(CardInfo *card, uint16_t minutes);

// 获取当前卡
CardInfo* CardSystem_GetCurrentCard(void);

//充值卡
uint8_t CardSystem_Recharge(CardInfo *card, uint32_t amount);

#endif /* __CARD_SYSTEM_H */

