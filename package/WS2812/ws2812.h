#include "hk32f10x.h"
#include "delay.h"
#define 	RGB_LED 	GPIO_Pin_9
#define		RGB_LED_HIGH	(GPIO_SetBits(GPIOA,RGB_LED))
#define 	RGB_LED_LOW		(GPIO_ResetBits(GPIOA,RGB_LED))

void RGB_LED_Init(void);
void RGB_LED_Write0(void);
void RGB_LED_Write1(void);
void RGB_LED_Reset(void);
void RGB_LED_Write_Byte(uint8_t byte);
void RGB_LED_Write_24Bits(uint8_t green,uint8_t red,uint8_t blue);
void RGB_LED_Red(void);
void RGB_LED_Green(void);
void RGB_LED_Blue(void);
