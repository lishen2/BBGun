#include "stm32f10x.h"
#include "public.h"
#include "button.h"

#define BUTTON_FIRE_PIN   GPIO_Pin_5
#define BUTTON_FIRE_PORT  GPIOB 
#define BUTTON_FIRE_CLOCK RCC_APB2Periph_GPIOB

#define BUTTON_FUNC_PIN   GPIO_Pin_9
#define BUTTON_FUNC_PORT  GPIOB 
#define BUTTON_FUNC_CLOCK RCC_APB2Periph_GPIOB

void button_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(BUTTON_FIRE_CLOCK|BUTTON_FUNC_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Pin = BUTTON_FIRE_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(BUTTON_FIRE_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = BUTTON_FUNC_PIN;
	GPIO_Init(BUTTON_FUNC_PORT, &GPIO_InitStructure);
	
	return;
}

int button_get(void)
{
	u8 hit;
	u8 key;

	key = BUTTON_NOKEY;

	//check fire key
	hit = GPIO_ReadInputDataBit(BUTTON_FIRE_PORT, BUTTON_FIRE_PIN);
	if (Bit_RESET == hit){

		delay_ms(20);
		hit = GPIO_ReadInputDataBit(BUTTON_FIRE_PORT, BUTTON_FIRE_PIN);
		if (Bit_RESET == hit){
			 key = BUTTON_FIRE;
		}
	}

	if (BUTTON_NOKEY != key){
		return key;
	}

	//check function key
	hit = GPIO_ReadInputDataBit(BUTTON_FUNC_PORT, BUTTON_FUNC_PIN);
	if (Bit_RESET == hit){

		delay_ms(20);
		hit = GPIO_ReadInputDataBit(BUTTON_FUNC_PORT, BUTTON_FUNC_PIN);
		if (Bit_RESET == hit){
			 key = BUTTON_RELOAD;
		}
	}

	return key;
}

