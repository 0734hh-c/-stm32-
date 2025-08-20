#ifndef __MENU_H
#define __MENU_H

#include "stm32f10x.h"
#include "OLED.h"
#include "key.h"
#include "card_system.h"
#include "dht11.h"
// 菜单状态枚举
typedef enum {
    MAIN_MENU,          // 主菜单
    CHARGE_SUB_MENU,    // 充电子菜单
    BALANCE_QUERY,      // 余额查询
    CHARGING,           // 充电中界面
    CHARGE_FINISH,       // 充电结束界面
    CARD_SCANNING,       // 刷卡中
    INSUFFICIENT_BALANCE // 余额不足
} MenuState;

// 充电时间选项
typedef enum {
    CHARGE_60MIN,
    CHARGE_180MIN,
    CHARGE_360MIN,
    CHARGE_600MIN,
    CHARGE_OPTION_COUNT
} ChargeOption;

// 菜单系统初始化
void Menu_Init(void);

// 菜单处理函数（在main循环中调用）
void Menu_Process(void);

// 设置充电剩余时间（单位：分钟）
void Menu_SetChargeTime(uint16_t minutes);

#endif /* __MENU_H */

