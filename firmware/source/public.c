#include <stdio.h>
#include "stm32f10x.h"
#include "public.h"
#include "ringbuf.h"
#include "usart_io.h"

#define PUBLIC_DELAY_TIMER_RCC          RCC_APB1Periph_TIM4
#define PUBLIC_DELAY_TIMER              TIM4
#define PUBLIC_DELAY_TIMER_FEQ          24000000
#define PUBLIC_DELAY_NS_PRE_TIMER_TICK  (1 * 1000 * 1000 * 1000 / PUBLIC_DELAY_TIMER_FEQ);

vu32 g_jiffies = 0;

void SysTick_Handler(void)
{
	++g_jiffies;
	return;
}

static inline void PWR_PrepareSleep(u32 SysCtrl_Set)
{
    if (SysCtrl_Set)
        SCB->SCR |= SCB_SCR_SLEEPONEXIT;   // Set SLEEPONEXIT                       
    else
        SCB->SCR &= ~SCB_SCR_SLEEPONEXIT;  // Reset SLEEPONEXIT

    SCB->SCR &= ~SCB_SCR_SLEEPDEEP;        // Clear SLEEPDEEP bit
}

//为了准确性，此函数需至少延时几百个纳秒，延时越长越精确
//但是延时时间最长不得长于 0x7FFFFFFF * PUBLIC_NS_PRE_TICK = 40802189293ns = 40s
void delay_ns(u32 ns)
{
    u32 period;

    period = ns / PUBLIC_DELAY_NS_PRE_TIMER_TICK;

    return;
}

void delay_ms(int ms)
{
	ms = ms/(1000/HZ) + g_jiffies;

	PWR_PrepareSleep(0);
	while(time_after(ms, g_jiffies)){
		__WFI();
	}

	return;
}

static _init_delay_timer(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    u16 prescaler;

    RCC_APB1PeriphClockCmd(PUBLIC_DELAY_TIMER_RCC, ENABLE);
    
    /* clock at 24 MHz */
	prescaler = (u16) (SystemCoreClock / PUBLIC_DELAY_TIMER_FEQ) - 1;
    TIM_TimeBaseStructure.TIM_Period = 0xFFFF;
    TIM_TimeBaseStructure.TIM_Prescaler = prescaler; 
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;  
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
    TIM_TimeBaseInit(PUBLIC_DELAY_TIMER, &TIM_TimeBaseStructure);  

    return;
}

void system_init(void)
{
	/* setup SysTick time for 10 ms */
	SysTick_Config(SystemCoreClock / 100);

	return;
}

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

PUTCHAR_PROTOTYPE
{
    usartio_sendchar(USART1, ch);
	return ch;
}

