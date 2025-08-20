#include "menu.h"
#include <string.h>
#include "delay.h"
#include <stdio.h>
#include "jsy_mk_1031.h"
#include "relay.h"
// 当前菜单状态
JSY_Data sensor_data = {
    .voltage = 20.0,
    .current = 1.0,
    .active_power = 50.0
};

static MenuState currentState = MAIN_MENU;
static MenuState prevState = MAIN_MENU; // 添加前一个状态记录

// 主菜单选项
static uint8_t mainMenuSelection = 0,result;
#define MAIN_MENU_OPTIONS 2  // "充电菜单"和"卡余额查询"

// 充电子菜单选项
static ChargeOption chargeSelection = CHARGE_60MIN;

// 充电剩余时间
static uint16_t remainingMinutes = 0;
static uint16_t del = 60;



// 当前卡片信息
static CardInfo currentCard = {
   .balance = 100,
    .secret = 0xff,
    .uid = 123
};



// 充电参数（模拟值）
static uint32_t voltage = 220;
static uint32_t current = 2;
static uint32_t power = 550;
static u8 temperature = 25;
static u8 humidity = 60;

// 显示主菜单
static void ShowMainMenu(void) {
    OLED_Clear();
    
    // 显示标题
    OLED_ShowChinese(20, 0, "充电桩系统");
    
    // 显示菜单选项
    OLED_ShowChinese(0, 20, "充电菜单");
    OLED_ShowChinese(0,40, "卡余额查询");
    
    // 显示选择光标
    if (mainMenuSelection == 0) {
        OLED_ShowString(90, 20, ">", OLED_8X16);
    } else {
        OLED_ShowString(90, 40, ">", OLED_8X16);
    }
}

// 显示充电子菜单
static void ShowChargeSubMenu(void) {
    OLED_Clear();
    // 显示充电选项
    OLED_ShowNum(0, 0, 60, 2, OLED_8X16);
    OLED_ShowChinese(30, 0, "分钟");
    OLED_ShowNum(0, 16, 180, 3, OLED_8X16);
    OLED_ShowChinese(30, 17, "分钟");
    OLED_ShowNum(0, 32, 360, 3, OLED_8X16);
    OLED_ShowChinese(30, 33, "分钟");
    OLED_ShowNum(0, 48, 600, 3, OLED_8X16);
    OLED_ShowChinese(30, 49, "分钟");
    // 显示选择光标
    switch (chargeSelection) {
        case CHARGE_60MIN:
            OLED_ShowString(90, 0, ">", OLED_8X16);
            break;
        case CHARGE_180MIN:
            OLED_ShowString(90, 16, ">", OLED_8X16);
            break;
        case CHARGE_360MIN:
            OLED_ShowString(90, 32, ">", OLED_8X16);
            break;
        case CHARGE_600MIN:
            OLED_ShowString(90, 48, ">", OLED_8X16);
            break;
        default:
            break;
    }
}

// 显示余额查询
static void ShowBalanceQuery(void) {
    OLED_Clear();

    if (currentCard.uid == 0) {
        OLED_ShowChinese(30, 2, "未检测到卡");
        return;
    }
    
     // 显示用户信息 - 直接使用32位整数显示
    char uidStr[9]; // 8字符UID + 终止符
    // 将32位UID格式化为8位十六进制字符串
    sprintf(uidStr, "%08lX", currentCard.uid);
    
    // 显示标题
    OLED_ShowChinese(20, 0, "余额查询");
    
    // 显示用户信息
    OLED_ShowChinese(0, 20, "用户");
    OLED_ShowString(50, 20, uidStr, OLED_8X16);

    // 显示余额信息
    OLED_ShowChinese(0, 40, "卡内余额");
    // 转换为元显示
    uint32_t yuan = currentCard.balance / 100;
    uint32_t fen = currentCard.balance % 100;
    
    char balanceStr[16];
    sprintf(balanceStr, "%d.%02d 元", yuan, fen);
    
    OLED_ShowString(70, 40, balanceStr, OLED_6X8);
}

// 显示充电中界面
static void ShowCharging(void) {
    OLED_Clear();
    
    // 显示标题
    OLED_ShowChinese(30, 0, "充电中");
    
    // 显示充电参数
    OLED_ShowChinese(0, 16, "电压");
    OLED_ShowNum(33, 16, voltage, 3, OLED_8X16);
    
    OLED_ShowChinese(65, 16, "电流");
    OLED_ShowNum(98, 16, current, 2, OLED_8X16);
    
    OLED_ShowChinese(0, 32, "功率");
    OLED_ShowNum(33, 32, power, 3, OLED_8X16);
    
    OLED_ShowChinese(65, 32, "温度");
    OLED_ShowNum(98,32, temperature, 2, OLED_8X16);
    
    OLED_ShowChinese(0, 48, "湿度");
    OLED_ShowNum(33, 48, humidity, 2, OLED_8X16);
    
    OLED_ShowChinese(65, 48, "剩余");
    OLED_ShowNum(98, 48, remainingMinutes, 3, OLED_8X16);
}

// 显示充电结束界面
static void ShowChargeFinish(void) {
    OLED_Clear();
	
    DHT11_Read_Data(&temperature,&humidity);
    // 显示标题
    OLED_ShowChinese(30, 0, "充电完成");
    
    // 显示充电参数
    OLED_ShowChinese(0, 16, "电压");
    OLED_ShowNum(33, 16, voltage, 3, OLED_8X16);
    
    OLED_ShowChinese(65, 16, "电流");
    OLED_ShowNum(98, 16, current, 2, OLED_8X16);
    
    OLED_ShowChinese(0, 32, "功率");
    OLED_ShowNum(33, 32, power, 3, OLED_8X16);
    
    OLED_ShowChinese(65, 32, "温度");
    OLED_ShowNum(98,32, temperature, 2, OLED_8X16);
    
    OLED_ShowChinese(0, 48, "湿度");
    OLED_ShowNum(33, 48, humidity, 2, OLED_8X16);
    
    OLED_ShowChinese(65, 48, "剩余");
    OLED_ShowNum(98, 48, remainingMinutes, 3, OLED_8X16);
}

// 菜单初始化
void Menu_Init(void) {
    currentState = MAIN_MENU;
    prevState = MAIN_MENU; // 初始化前一个状态
    mainMenuSelection = 0;
    chargeSelection = CHARGE_60MIN;
    remainingMinutes = 0;
    
    // 初始化卡片信息
    memset(&currentCard, 0, sizeof(CardInfo));
    
    // 初始化显示
    ShowMainMenu();
    OLED_Update();
}

// 设置充电剩余时间
void Menu_SetChargeTime(uint16_t minutes) {
    remainingMinutes = minutes;
}

// 菜单处理函数
void Menu_Process(void) {
    static uint8_t keyPressed = 0;
     CardInfo card;
    // 在特定状态检测卡片
    if (currentState == CARD_SCANNING && CardSystem_ReadCard(&card)) 
	{
		printf("检查卡片中\r\n");
		currentCard = card;
         printf("card ID = %s\r\n",card.uid);
        if (prevState == CHARGE_SUB_MENU) 
		{
			printf("充电场景\r\n");
            // 充电场景
            if (chargeSelection != CHARGE_OPTION_COUNT) 
			{
                uint16_t chargeTime = 0;
                switch(chargeSelection) 
				{
                    case CHARGE_60MIN: chargeTime = 60; break;
                    case CHARGE_180MIN: chargeTime = 180; break;
                    case CHARGE_360MIN: chargeTime = 360; break;
                    case CHARGE_600MIN: chargeTime = 600; break;
                    default: break;
                }
                
                if (CardSystem_Charge(&currentCard, chargeTime)) 
				{
                    remainingMinutes = chargeTime;
                    currentState = CHARGING;
                    ShowCharging();
                } 
				else 
				{
                    currentState = INSUFFICIENT_BALANCE;
                    // 显示余额不足界面
                    OLED_Clear();
                    OLED_ShowChinese(30, 20, "余额不足");
                    OLED_ShowChinese(20, 40, "请充值后再试");
                }
            } 
            // 余额查询场景
        }
		 else 
		{
			printf("余额查询场景\r\n");
			if(CardSystem_Recharge(&card,100))
			{
				printf("充值成功\r\n");
			}
			else
			{
				printf("充值失败\r\n");
			}
	        currentState = BALANCE_QUERY;
	        ShowBalanceQuery();
	    }
    }
	
    
    // 读取按键状态
    uint8_t key1 = KEY_Status(KEY1);
    uint8_t key2 = KEY_Status(KEY2);
    uint8_t key3 = KEY_Status(KEY3);
	delay_ms(10);

    // 按键1处理（确认）
    if (key1 == 0 && keyPressed == 0) {
        keyPressed = 1;
        
        switch (currentState) {
            case MAIN_MENU:
                if (mainMenuSelection == 0) {
                    // 进入充电子菜单
                    prevState = currentState; // 保存前一个状态
                    currentState = CHARGE_SUB_MENU;
                    ShowChargeSubMenu();
                } else {
                    // 进入余额查询需要刷卡
                    prevState = currentState; // 保存前一个状态
                    currentState = CARD_SCANNING;
                    // 显示刷卡提示
                    OLED_Clear();
                    OLED_ShowChinese(25, 0, "请刷卡查询");
                }
                break;
                
            case CHARGE_SUB_MENU:
                 // 进入充电需要刷卡
                prevState = currentState; // 保存前一个状态
                currentState = CARD_SCANNING;
                // 显示刷卡提示
                OLED_Clear();
                OLED_ShowChinese(25, 0, "请刷卡充电");
                break;
                
            case BALANCE_QUERY:
            case CHARGE_FINISH:
            case INSUFFICIENT_BALANCE:
                // 返回主菜单
                currentState = MAIN_MENU;
                ShowMainMenu();
                break;
                
            case CHARGING:
                // 充电中按确认键无操作
                break;
        }
    }
    // 按键2处理（选择/下移）
    else if (key2 == 0 && keyPressed == 0) {
        keyPressed = 1;
        
        switch (currentState) {
            case MAIN_MENU:
                mainMenuSelection = (mainMenuSelection + 1) % MAIN_MENU_OPTIONS;
                ShowMainMenu();
                break;
                
            case CHARGE_SUB_MENU:
                // 修复枚举类型警告
                chargeSelection = (ChargeOption)((chargeSelection + 1) % CHARGE_OPTION_COUNT);
                ShowChargeSubMenu();
                break;
                
            default:
                break;
        }
    }
    // 按键3处理（返回/上移）
    else if (key3 == 0 && keyPressed == 0) {
        keyPressed = 1;
        
        switch (currentState) {
            case MAIN_MENU:
                // 主菜单按返回键无操作
                break;
                
            case CHARGE_SUB_MENU:
                // 返回主菜单
                currentState = MAIN_MENU;
                ShowMainMenu();
                break;
                
            case BALANCE_QUERY:
            case INSUFFICIENT_BALANCE:
            case CHARGE_FINISH:
                // 返回主菜单
                currentState = MAIN_MENU;
                ShowMainMenu();
                break;
                
            case CHARGING:
                // 充电中按返回键取消充电
                //currentState = MAIN_MENU;
                //ShowMainMenu();
                break;
                
            // 新增：在刷卡过程中按返回键
            case CARD_SCANNING:
                // 返回前一个状态
                currentState = prevState;
                // 清除卡片信息
                memset(&currentCard, 0, sizeof(CardInfo));
                
                // 根据前一个状态显示相应界面
                if (prevState == MAIN_MENU) {
                    ShowMainMenu();
                } else if (prevState == CHARGE_SUB_MENU) {
                    ShowChargeSubMenu();
                }
                break;
        }
    }
    
    // 按键释放检测
    if (key1 && key2 && key3) {
        keyPressed = 0;
    }
    
    // 充电中状态处理
    if (currentState == CHARGING && remainingMinutes > 0) 
	{
		Relay_On();
		del--;
    	delay_ms(1000);
		if(del == 0)
		{
			remainingMinutes--;
			del =60;
		}        
        
        result = JSY_ReadData(USART2, 0x01, &sensor_data);
        delay_ms(1000);
        if (result == JSY_OK) {
            printf("Voltage: %.2f V\r\n", sensor_data.voltage);
            printf("Current: %.4f A\r\n", sensor_data.current);
            printf("Power: %.4f W\r\n", sensor_data.active_power);
        } else {
            printf("JSY Error: %d\r\n", result);
        }
		voltage = (uint32_t)sensor_data.voltage;
		current = (uint32_t)sensor_data.current;
		power = (uint32_t)sensor_data.active_power;
        DHT11_Read_Data(&temperature,&humidity);
		printf("电压 = %d,电流 = %d,功率 = %d\r\n",voltage,current,power);
        printf("temp = %d,humi = %d\r\n",temperature,humidity);
        ShowCharging();
        // 充电完成
        if (remainingMinutes == 0 ||voltage > 230 || current > 10 || power >2300) {
            currentState = CHARGE_FINISH;
			Relay_Off();
            ShowChargeFinish();
        }
    }
    
    // 更新OLED显示
    OLED_Update();
}

