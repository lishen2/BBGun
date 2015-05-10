#include <string.h>            
#include "stm32f10x.h"
#include "public.h"
#include "led.h"

/* tm1809 control pin */
#define LED_CTL_PIN   GPIO_Pin_8
#define LED_CTL_PORT  GPIOB 
#define LED_CTL_CLOCK RCC_APB2Periph_GPIOB

/* pin control */
#define LED_SET_HIGH() LED_CTL_PORT->BSRR = LED_CTL_PIN
#define LED_SET_LOW()  LED_CTL_PORT->BRR = LED_CTL_PIN

//delay
#define LED_DELAY(COUNT)  for(counter = 0; counter < COUNT; ++counter)

//progress led data count and start index
#define LED_PROGRESS_COUNT 6
#define LED_PROGRESS_START 0
#define LED_RGB_COUNT      3

//RGB led data indexes
#define LED_RGB_RED   (LED_PROGRESS_COUNT + 0)
#define LED_RGB_GREEN (LED_PROGRESS_COUNT + 1)
#define LED_RGB_BLUE  (LED_PROGRESS_COUNT + 2)

/* display buffer, 0-5 for six progress led, 6-8 for RGB led */
static unsigned char g_ledbuf[LED_PROGRESS_COUNT + LED_RGB_COUNT];
static unsigned char g_brightness[LED_PROGRESS_COUNT + LED_RGB_COUNT] = {0x30, 0x30, 0x30, 0xA0, 0xA0, 0xFF, 0x50, 0x40, 0x45};

static void _update_led(void)
{
	int i, counter;
	unsigned char ch;
		 
    __disable_irq();

	for (i = 0; i < sizeof(g_ledbuf);){
	
		ch = g_ledbuf[i];
		if (ch & 0x80) {
			LED_SET_HIGH();
			LED_DELAY(7);
			LED_SET_LOW();
			LED_DELAY(1);
		} else {
			LED_SET_HIGH();
			LED_DELAY(2);
			LED_SET_LOW();
			LED_DELAY(6);		  
		}

		if (ch & 0x40) {
			LED_SET_HIGH();
			LED_DELAY(7);
			LED_SET_LOW();
			LED_DELAY(1);
		} else {
			LED_SET_HIGH();
			LED_DELAY(2);
			LED_SET_LOW();
			LED_DELAY(6);		  
		}

		if (ch & 0x20) { 
			LED_SET_HIGH();
			LED_DELAY(7);
			LED_SET_LOW();
			LED_DELAY(1);
		} else {
			LED_SET_HIGH();
			LED_DELAY(2);
			LED_SET_LOW();
			LED_DELAY(6);		  
		}

		if (ch & 0x10) {
			LED_SET_HIGH();
			LED_DELAY(7);
			LED_SET_LOW();
			LED_DELAY(1);
		} else {
			LED_SET_HIGH();
			LED_DELAY(2);
			LED_SET_LOW();
			LED_DELAY(6);		  
		}

		if (ch & 0x08) {
			LED_SET_HIGH();
			LED_DELAY(7);
			LED_SET_LOW();
			LED_DELAY(1);
		} else {
			LED_SET_HIGH();
			LED_DELAY(2);
			LED_SET_LOW();
			LED_DELAY(6);		  
		}

		if (ch & 0x04) {
			LED_SET_HIGH();
			LED_DELAY(7);
			LED_SET_LOW();
			LED_DELAY(1);
		} else {
			LED_SET_HIGH();
			LED_DELAY(2);
			LED_SET_LOW();
			LED_DELAY(6);		  
		}

		if (ch & 0x02) {
			LED_SET_HIGH();
			LED_DELAY(7);
			LED_SET_LOW();
			LED_DELAY(1);
		} else {
			LED_SET_HIGH();
			LED_DELAY(2);
			LED_SET_LOW();
			LED_DELAY(6);		  
		}

		++i;

		if (ch & 0x01) {
			LED_SET_HIGH();
			LED_DELAY(7);
			LED_SET_LOW();
		} else {
			LED_SET_HIGH();
			LED_DELAY(2);
			LED_SET_LOW();
			LED_DELAY(4);		  
		}
	}

	//send reset(low for a while)
	LED_SET_LOW();

	__enable_irq();

	delay_ms(10);

	return;
}

/* set color of RGB led */
void led_setcolor(unsigned int color)
{
	switch(color)
	{
	case LED_COLOR_RED:
		g_ledbuf[LED_RGB_RED]   = g_brightness[LED_RGB_RED];
		g_ledbuf[LED_RGB_GREEN] = 0;
		g_ledbuf[LED_RGB_BLUE]  = 0;
		break;
	case LED_COLOR_GREEN: 
		g_ledbuf[LED_RGB_RED]   = 0;
		g_ledbuf[LED_RGB_GREEN] = g_brightness[LED_RGB_GREEN];
		g_ledbuf[LED_RGB_BLUE]  = 0;
		break;   
	case LED_COLOR_BLUE: 
	 	g_ledbuf[LED_RGB_RED]   = 0;
		g_ledbuf[LED_RGB_GREEN] = 0;
		g_ledbuf[LED_RGB_BLUE]  = g_brightness[LED_RGB_BLUE];
		break;   
	case LED_COLOR_YELLOW:
		g_ledbuf[LED_RGB_RED]   = g_brightness[LED_RGB_RED];
		g_ledbuf[LED_RGB_GREEN] = g_brightness[LED_RGB_GREEN];
		g_ledbuf[LED_RGB_BLUE]  = 0;
		break;   
	case LED_COLOR_CYAN:
		g_ledbuf[LED_RGB_RED]   = 0;
		g_ledbuf[LED_RGB_GREEN] = g_brightness[LED_RGB_GREEN];
		g_ledbuf[LED_RGB_BLUE]  = g_brightness[LED_RGB_BLUE];	  
		break;  
	case LED_COLOR_MEGENTA:
		g_ledbuf[LED_RGB_RED]   = g_brightness[LED_RGB_RED];
		g_ledbuf[LED_RGB_GREEN] = 0;
		g_ledbuf[LED_RGB_BLUE]  = g_brightness[LED_RGB_BLUE];
		break;  
	case LED_COLOR_WRITE:
		g_ledbuf[LED_RGB_RED]   = g_brightness[LED_RGB_RED];
		g_ledbuf[LED_RGB_GREEN] = g_brightness[LED_RGB_GREEN];
		g_ledbuf[LED_RGB_BLUE]  = g_brightness[LED_RGB_BLUE];
		break;
	default:
		g_ledbuf[LED_RGB_RED]   = 0;
		g_ledbuf[LED_RGB_GREEN] = 0;
		g_ledbuf[LED_RGB_BLUE]  = 0;
		break;
	}

	_update_led();
	return;
}

/* set the number of lighten leds */			  
void led_setbar(int count)
{
	memset(g_ledbuf + LED_PROGRESS_START, 0, LED_PROGRESS_COUNT);
	
	if (count > LED_PROGRESS_COUNT){
		 count = LED_PROGRESS_COUNT;
	}

	memcpy(g_ledbuf, g_brightness, count);
	
	_update_led();
	return;
}

void led_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(LED_CTL_CLOCK, ENABLE);
	GPIO_InitStructure.GPIO_Pin = LED_CTL_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_Init(LED_CTL_PORT, &GPIO_InitStructure);	

	LED_SET_LOW();
	delay_ms(50);

	memset(g_ledbuf, 0, sizeof(g_ledbuf));
	_update_led();

	return;
}

