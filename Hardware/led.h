#ifndef _LED_H__
#define _LED_H__
/*√∂æŸ∂®“ÂLEDµ∆*/

typedef enum {D0,D1,D2,D3} LED_NUM;
typedef enum {LED_ON,LED_OFF} LED_STATUS;

void LED_Init(void);
void LED_Ctl(LED_NUM led_num,LED_STATUS led_status);
void LED_SET(void);

#define D0_ON LED_Ctl(D0,LED_ON)
#define D0_OFF LED_Ctl(D0,LED_OFF)

#define D1_ON LED_Ctl(D1,LED_ON)
#define D1_OFF LED_Ctl(D1,LED_OFF)

#define D2_ON LED_Ctl(D2,LED_ON)
#define D2_OFF LED_Ctl(D2,LED_OFF)

#define D3_ON LED_Ctl(D3,LED_ON)
#define D3_OFF LED_Ctl(D3,LED_OFF)

#define LED_ALL_ON  (D0_ON,D1_ON,D2_ON,D3_ON)
#define LED_ALL_OFF (D0_OFF,D1_OFF,D2_OFF,D3_OFF)

#define LED_ALL_SET LED_SET()
#endif
