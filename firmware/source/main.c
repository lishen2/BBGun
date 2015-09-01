#include <stdio.h>
#include "stm32f10x.h"
#include "public.h"
#include "led.h"
#include "ir.h"
#include "menu.h"

int main()
{
	system_init();
	usartio_init_usart1(); 
	usartio_init_usart2();
	usartio_init_usart3();
	button_init();
	ir_init();
    
	printf("started\n");

	while(1){
        ir_handle_msg();
        menu_handle_key();

		delay_ms(20);
	}

	return 0;
}

