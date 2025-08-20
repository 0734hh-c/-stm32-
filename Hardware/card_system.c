#include "card_system.h"
#include <string.h>

// 当前卡信息
static CardInfo currentCard;
static uint8_t cardPresent = 0;

// W25Q64存储配置
#define CARD_STORAGE_BASE_ADDR  0x000000
#define CARD_DATA_SIZE          sizeof(CardInfo)

// 卡系统初始化
void CardSystem_Init(void) {
    MFRC522_Init();
    MFRC522_Reset();
    MFRC522_AntennaOn();
    memset(&currentCard, 0, sizeof(CardInfo));
	W25Q64_Init();
}

// 检测是否有卡
uint8_t CardSystem_DetectCard(void) {
    uint8_t status,temp,i;
    uint8_t tagType[2];
    
    // 寻卡
    status = MFRC522_Request(PICC_REQIDL, tagType);
    if (status != MI_OK) return 0;
    
	printf("卡的类型:");
	for(i=0;i<2;i++)
	{
			temp=tagType[i];
			printf("%X",temp);
	}
				
	printf("\r\n");
	
    // 防冲突
    status = MFRC522_Anticoll(currentCard.uid);
    if (status != MI_OK) return 0;
    printf("card uid = %s\r\n",currentCard.uid);
    // 选卡
    status = MFRC522_SelectTag(currentCard.uid);
    return (status == MI_OK);
}

// 读取卡信息
uint8_t CardSystem_ReadCard(CardInfo *card) {
    if (!CardSystem_DetectCard()) 
    {
    	printf("未找到卡\r\n");
		return 0;
	}
		
    printf("card uid = %s\r\n",card->uid);
    // 从W25Q64读取卡数据
    W25Q64_ReadData(CARD_STORAGE_BASE_ADDR + 
                   (currentCard.uid[0] << 16) + 
                   (currentCard.uid[1] << 8) + 
                   currentCard.uid[2], 
                   (uint8_t*)card, 
                   CARD_DATA_SIZE);
    printf("W25Q64读卡结束\r\n");
    // 验证UID是否匹配
    if (memcmp(card->uid, currentCard.uid, 4) != 0) {
        // 新卡，初始化默认值
        memcpy(card->uid, currentCard.uid, 4);
        card->balance = 10;
        memset(card->secret, 0xFF, 6); // 默认密码0xFFFFFFFFFFFF
        printf("card uid = %s\r\n",card->uid);
        CardSystem_SaveCard(card);
    }
    
    cardPresent = 1;
    return 1;
}

// 保存卡信息
void CardSystem_SaveCard(CardInfo *card) {
    // 计算存储地址 (基于UID)
    uint32_t addr = CARD_STORAGE_BASE_ADDR + 
                   (card->uid[0] << 16) + 
                   (card->uid[1] << 8) + 
                   card->uid[2];
    
    // 擦除扇区 (4KB)
    W25Q64_SectorErase(addr & 0xFFF000);
    
    // 写入卡数据
    W25Q64_PageProgram(addr, (uint8_t*)card, CARD_DATA_SIZE);
}

// 消费处理
uint8_t CardSystem_Charge(CardInfo *card, uint16_t minutes) {
    // 计算费用 (1元/小时)
    uint32_t cost = (minutes + 59) / 60 * 100; // 转换为分
    
    if (card->balance < cost) {
        return 0; // 余额不足
    }
    
    // 扣除费用
    card->balance -= cost;
    
    // 保存更新后的卡信息
    CardSystem_SaveCard(card);
    return 1;
}

// 充值函数 
uint8_t CardSystem_Recharge(CardInfo *card, uint32_t amount) 
{
    // 检查卡片有效性
    if (card->uid[0] == 0 && card->uid[1] == 0 && card->uid[2] == 0 && card->uid[3] == 0) {
        return 0; // 无效卡片
    }
    
    // 增加余额 (amount单位:分)
    card->balance += amount;
    
    // 保存更新后的卡信息
    CardSystem_SaveCard(card);
    
    return 1; // 充值成功
}

// 获取当前卡
CardInfo* CardSystem_GetCurrentCard(void) {
    return cardPresent ? &currentCard : NULL;
}

