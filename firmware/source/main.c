#include <stdio.h>
#include "stm32f10x.h"
#include "public.h"
#include "usart_io.h"
#include "buzzer.h"
#include "button.h"
#include "led.h"
#include "ir.h"
#include "switch.h"

int main()
{
	int ret;
	int key;
	unsigned char team, persion, damage;
	unsigned char myteam;

	system_init();
	usartio_init_usart1(); 
	usartio_init_usart2();
	usartio_init_usart3();
	button_init();
//	buz_init();
	ir_init();

	buz_beep(BUZ_SOUND_BEGIN);
	printf("started\n");

	myteam = switch_getvalue();

	while(1){
		key = button_get();
		if (BUTTON_FIRE == key){
			IR_FIRE_RUFKE(myteam, 0x00);
//			buz_beep(BUZ_SOUND_FIRE);
			printf("Fire\n");
		}

		ret = ir_ishit(&team, &persion, &damage);
		if (ERR_OK == ret){
//			buz_beep(BUZ_SOUND_HIT);
			printf("has been hit, team[%hhu] persion[%hhu] damage[%hhu]\n", team, persion, damage);
		}

		delay_ms(20);
	}

	return 0;
}

