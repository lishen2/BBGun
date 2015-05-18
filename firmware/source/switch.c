#include "stm32f10x.h"
#include "public.h"
#include "switch.h"

#define SWITCH_GPIO_RCC   RCC_APB2Periph_GPIOB
#define SWITCH_GPIO_PORT  GPIOB
#define SWITCH_GPIO_PIN1  GPIO_Pin_12
#define SWITCH_GPIO_PIN2  GPIO_Pin_13
#define SWITCH_GPIO_PIN3  GPIO_Pin_14

static void _init_pin(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(SWITCH_GPIO_RCC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = SWITCH_GPIO_PIN1 | SWITCH_GPIO_PIN2 | SWITCH_GPIO_PIN3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(SWITCH_GPIO_PORT, &GPIO_InitStructure);

	return;		
}

static void _deinit_pin(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(SWITCH_GPIO_RCC, ENABLE);
	GPIO_InitStructure.GPIO_Pin = SWITCH_GPIO_PIN1 | SWITCH_GPIO_PIN2 | SWITCH_GPIO_PIN3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(SWITCH_GPIO_PORT, &GPIO_InitStructure);

	return;
}

unsigned char switch_getvalue(void)
{
	unsigned char val = 0;
	unsigned char ret;

	_init_pin();	
	delay_ms(10);

	ret = GPIO_ReadInputDataBit(SWITCH_GPIO_PORT, SWITCH_GPIO_PIN1);
	if (Bit_SET == ret){
		val	|= 1 << 0;
	}

	ret = GPIO_ReadInputDataBit(SWITCH_GPIO_PORT, SWITCH_GPIO_PIN2);
	if (Bit_SET == ret){
		val	|= 1 << 1;
	}
	
	ret = GPIO_ReadInputDataBit(SWITCH_GPIO_PORT, SWITCH_GPIO_PIN3);
	if (Bit_SET == ret){
		val	|= 1 << 2;
	}		

	_deinit_pin();

	return val;
}

