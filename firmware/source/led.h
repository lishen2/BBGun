#ifndef _LED_H_
#define _LED_H_

enum {
	LED_COLOR_RED,
	LED_COLOR_GREEN,    
	LED_COLOR_BLUE,     
	LED_COLOR_YELLOW,   
	LED_COLOR_CYAN,    
	LED_COLOR_MEGENTA,  
	LED_COLOR_WRITE,	
};

void led_setcolor(unsigned int color);
void led_setbar(int count);
void led_init(void);

#endif

