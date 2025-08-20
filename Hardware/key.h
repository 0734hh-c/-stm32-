#ifndef KEY_H
#define KEY_H
#include "stdint.h"
/*定义按键 和按键状态*/

typedef enum {KEY1,KEY2,KEY3} KEY_NUM; 
typedef enum {KEY_DOWN,KEY_UP,KEY_NUM_ERROR}  KEY_STATE;
void KEY_Init(void);

uint8_t KEY_Status(KEY_NUM key_num);

#define KEY1_CTL KEY_Status(KEY1)
#define KEY2_CTL KEY_Status(KEY2)
#define KEY3_CTL KEY_Status(KEY3)

#endif 

