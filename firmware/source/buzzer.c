#include "stm32f10x.h"
#include "public.h"
#include "buzzer.h"

//define buzzer control pin
#define BUZ_CTL_PIN     GPIO_Pin_2
#define BUZ_CTL_PORT    GPIOA 
#define BUZ_CTL_CLOCK   RCC_APB2Periph_GPIOA
#define BUZ_TIMER       TIM2
#define BUZ_TIMER_CLOCK RCC_APB1Periph_TIM2

static void _config_pwm(uint16_t freq)
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
	period = 24000000/freq - 1;
	
	/* Time base configuration */
	TIM_TimeBaseStructure.TIM_Period = period;
	TIM_TimeBaseStructure.TIM_Prescaler = prescaler;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	
	TIM_TimeBaseInit(BUZ_TIMER, &TIM_TimeBaseStructure);
	
	/* PWM1 Mode configuration */
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_Pulse = period>>1;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC3Init(BUZ_TIMER, &TIM_OCInitStructure);
	TIM_OC3PreloadConfig(BUZ_TIMER, TIM_OCPreload_Enable);

	TIM_ARRPreloadConfig(BUZ_TIMER, ENABLE);
	/* enable counter */
	TIM_Cmd(BUZ_TIMER, ENABLE);

	return;
}

static void _stop_pwd(void)
{
	TIM_Cmd(BUZ_TIMER, DISABLE);

	return;
}

void buz_beep(int type)
{
	switch(type)
	{
	case BUZ_SOUND_BEGIN:
		_config_pwm(500);
		delay_ms(200);
		_config_pwm(1000);
		delay_ms(200);
		break;
	case BUZ_SOUND_DEAD:
		_config_pwm(1100);
		delay_ms(500);
		_config_pwm(500);
		delay_ms(500);
		_config_pwm(400);
		delay_ms(500);
		_config_pwm(200);
		delay_ms(500);
		break;
	case BUZ_SOUND_EMPTY:
		_config_pwm(500);
		delay_ms(100);
		break;
	case BUZ_SOUND_HIT:
		_config_pwm(2000);
		delay_ms(200);
		_config_pwm(1000);
		delay_ms(100);
		break;
	case BUZ_SOUND_FIRE:
		_config_pwm(2000);
		delay_ms(200);	
		_config_pwm(2500);
		delay_ms(200);					
		break;
	default:
		break;
	}

	_stop_pwd();

	return;
}

void buz_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/* timer clock enable */
	RCC_APB1PeriphClockCmd(BUZ_TIMER_CLOCK, ENABLE);
	
	/* GPIO clock enable */
	RCC_APB2PeriphClockCmd(BUZ_CTL_CLOCK, ENABLE);

	/* BUZZER gpio */
	GPIO_InitStructure.GPIO_Pin = BUZ_CTL_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(BUZ_CTL_PORT, &GPIO_InitStructure);

	return;
}

