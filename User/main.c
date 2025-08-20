#include "stm32f10x.h"                  // Device header
#include "delay.h"
#include "menu.h"
#include "dht11.h"
#include "usart.h"
#include "beep.h"
#include "flame_sensor.h"
#include "jsy_mk_1031.h"
#include "relay.h"
int main(void)
{
	
	Beep_Init();
	OLED_Init();
	KEY_Init();
	Menu_Init();
	CardSystem_Init(); 
	USART1_Init(9600);
	USART2_Init(9600);
	DHT11_Init();
	MFRC522_Init();
	FlameSensor_Init();
	Relay_Init();
	while(1)
	{
		
		Menu_Process();
		FlameSensor_Waring();
	}
	
}
