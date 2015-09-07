#include <stdio.h>
#include "stm32f10x.h"
#include "public.h"
#include "led.h"
#include "ir.h"
#include "menu.h"
#include "ringbuf.h"
#include "usart_io.h"
#include "button.h"

int main()
{
	system_init();
	usartio_init_usart1(); 
	usartio_init_usart2();
	usartio_init_usart3();
	button_init();
	ir_init();
    
	printf("started\n");

	ir_send_shot(0x01, 0x02, 0x03);
	while(1){
		
        ir_handle_msg();
        logic_handle_key();

		delay_ms(20);
	}

	return 0;
}

