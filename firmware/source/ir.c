#include "stm32f10x.h"
#include "public.h"
#include "usart_io.h"
#include "ir.h"

#define IR_FIRE_USART   USART3
#define IR_RECV1_UASRT  USART2 
#define IR_RECV2_UASRT  USART3

//38KHZ PWM pin
#define IR_PWM_PIN         GPIO_Pin_6
#define IR_PWM_PORT        GPIOA 
#define IR_PWM_PORT_CLOCK  RCC_APB2Periph_GPIOA
#define IR_PWM_TIMER       TIM3
#define IR_PWM_TIMER_CLOCK RCC_APB1Periph_TIM3

#define IR_PROTOCOL_MARK 0x7A

static int g_last_ts = 0;

static void _config_pwm(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	uint16_t prescaler;
	uint16_t period;

  /* -----------------------------------------------------------------------
    TIMMER Configuration: generate a signals with 50% duty cycles:
    The TIMCLK frequency is set to SystemCoreClock (Hz), to get TIMMER counter
    clock at 24 MHz the Prescaler is computed as following:
     - Prescaler = (TIMCLK / TIM counter clock) - 1
    SystemCoreClock is set to 72 MHz for Low-density, Medium-density, High-density
    and Connectivity line devices and to 24 MHz for Low-Density Value line and
    Medium-Density Value line devices

    The TIM is running at 38 KHz: TIM Frequency = TIM counter clock/(ARR + 1)
                                                  = 24 MHz / 666 = 38 KHz
    TIM duty cycle = (TIMX_CCR1/ TIMX_ARR)* 100 = 50%
  ----------------------------------------------------------------------- */

	/* Compute the prescaler and period */
	prescaler = (uint16_t) (SystemCoreClock / 24000000) - 1;
	period = 24000000/38000 - 1;
	
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = period;
	TIM_TimeBaseStructure.TIM_Prescaler = prescaler;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	
	TIM_TimeBaseInit(IR_PWM_TIMER, &TIM_TimeBaseStructure);
	
	/* PWM1 Mode configuration */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = period>>1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(IR_PWM_TIMER, &TIM_OCInitStructure);
	TIM_OC1PreloadConfig(IR_PWM_TIMER, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(IR_PWM_TIMER, ENABLE);
	/* TIM3 enable counter */
	TIM_Cmd(IR_PWM_TIMER, ENABLE);

	return;
}

void static _gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* timer clock enable */
	RCC_APB1PeriphClockCmd(IR_PWM_TIMER_CLOCK, ENABLE);
	
	/* GPIO clock enable */
	RCC_APB2PeriphClockCmd(IR_PWM_PORT_CLOCK, ENABLE);

	/* ir gpio */
	GPIO_InitStructure.GPIO_Pin = IR_PWM_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IR_PWM_PORT, &GPIO_InitStructure);

	return;
}

void ir_init(void)
{
	_gpio_init();
	_config_pwm();

	//keep IR diode from continuing emmit, disable pin here, enable when need
	usartio_disable_usart3_tx();

	return;
}

void ir_fire(unsigned char team, unsigned char persion, unsigned char damage, unsigned char firetime)
{
	if ((g_jiffies - g_last_ts)*(1000/HZ) < firetime){
		return;
	}

	usartio_enable_usart3_tx();
	delay_ms(20);

	usartio_sendchar_polling(IR_FIRE_USART, IR_PROTOCOL_MARK);
	usartio_sendchar_polling(IR_FIRE_USART, team);
	usartio_sendchar_polling(IR_FIRE_USART, damage);
	g_last_ts = firetime;
	
	usartio_disable_usart3_tx();

	return;	
}

int ir_ishit(unsigned char *team, unsigned char *persion, unsigned char *damage)
{
	unsigned char buf[3];
	int ret;

	buf[0] = 0;

	if (0 == buf[0] && usartio_receive_count(IR_RECV1_UASRT) >= 3){
		usartio_recvchar(IR_RECV1_UASRT, buf + 0);
		usartio_recvchar(IR_RECV1_UASRT, buf + 1);
		usartio_recvchar(IR_RECV1_UASRT, buf + 2);
	}
	
	if (0 == buf[0] && usartio_receive_count(IR_RECV2_UASRT) >= 3){
		usartio_recvchar(IR_RECV2_UASRT, buf + 0);
		usartio_recvchar(IR_RECV2_UASRT, buf + 1);
		usartio_recvchar(IR_RECV2_UASRT, buf + 2);			
	}

	if (IR_PROTOCOL_MARK == buf[0]){
		*team = buf[1];
		*damage = buf[2];
		ret = ERR_OK;
	} else {
		ret = ERR_FAILED;
	}

	return ret;
}

