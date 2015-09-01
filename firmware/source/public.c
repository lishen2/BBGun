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

void delay_ms(int ms)
{
	ms = ms/(1000/HZ) + g_jiffies;

	PWR_PrepareSleep(0);
	while(time_after(ms, g_jiffies)){
		__WFI();
	}

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

