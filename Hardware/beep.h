#ifndef _BEEP__H
#define _BEEP__H

void Beep_Init(void);

#define B0 GPIO_ResetBits(GPIOA,GPIO_Pin_8)
#define B1 GPIO_SetBits(GPIOA,GPIO_Pin_8)
void Beep_Turn(void);






#endif

