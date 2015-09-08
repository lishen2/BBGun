#include "stm32f10x.h"
#include "public.h"

//timer used to generate pwm signal
#define SND_PWM_TIMER_BASE               TIM3
#define SND_PWM_TIMER_OC_INIT(a1, a2)    TIM_OC3Init(a1,a2)
#define SND_PWM_TIMER_OC_PRELOAD(a1, a2) TIM_OC3PreloadConfig(a1, a2)
#define SND_PWM_OUTPUT_PIN               GPIO_Pin_0
#define SND_PWM_OUTPUT_PORT              GPIOB
#define SND_PWM_OUTPUT_PORT_CLK          RCC_APB2Periph_GPIOB

//timer used to generate sample event
#define SND_SAMPLE_TIMER_BASE    TIM2
#define SND_SAMPLE_IRQ_HANDLER   TIM2_IRQHandler

//sound buf size, sound sample rate is 8KHZ, so 8000 sample store 1 second sound
#define SND_SAMPLE_SIZE   8192

static uint8_t g_soundBuf[SND_SAMPLE_SIZE];
static uint32_t g_soundBufPos;

static void _gpioInit(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(SND_PWM_OUTPUT_PORT_CLK, ENABLE);
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin = SND_PWM_OUTPUT_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SND_PWM_OUTPUT_PORT, &GPIO_InitStructure);

    return;
}

static void _timerInit(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef        TIM_OCInitStructure;

    //sample timer work at 72MHZ (Prescaler is 0)
    //it's interrupt rate is 8khz  (72m/9000)
    TIM_TimeBaseStructure.TIM_Period = 9000;  
    TIM_TimeBaseStructure.TIM_Prescaler = 0x00; 
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;   
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; 
    TIM_TimeBaseInit(SND_SAMPLE_TIMER_BASE, &TIM_TimeBaseStructure);
    
    //pwm timer work at 72MHZ (Prescaler is 0)
    //it's PWM frequency : 281.250KHz (72MHZ/256)
    TIM_TimeBaseStructure.TIM_Period = 0xFF;
    TIM_TimeBaseStructure.TIM_Prescaler = 0;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(SND_PWM_TIMER_BASE, &TIM_TimeBaseStructure);    

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0x00;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    SND_PWM_TIMER_OC_INIT(SND_PWM_TIMER_BASE, &TIM_OCInitStructure);
    SND_PWM_TIMER_OC_PRELOAD(SND_PWM_TIMER_BASE, TIM_OCPreload_Enable);

    TIM_Cmd(SND_SAMPLE_TIMER_BASE, DISABLE);
    TIM_ITConfig(SND_SAMPLE_TIMER_BASE, TIM_IT_Update, ENABLE);

    TIM_Cmd(SND_PWM_TIMER_BASE, DISABLE);

    return;
}

void sound_init(void)
{
    _gpioInit();
    _timerInit();

    g_soundBufPos = SND_SAMPLE_SIZE;
    
    return;
}

void sound_play(int effect)
{
    uint32_t addr;
    int ret;

    if (ef >= SND_END){
        return;
    }

    //wait last sound finish
    while(SND_SAMPLE_SIZE != g_soundBufPos);

    //read sound data
    addr = effect * SND_SAMPLE_SIZE;
    ret = flash_spi_read(addr, SND_SAMPLE_SIZE, g_soundBuf);
    if (ERR_OK != ret){
        return;
    }

    g_soundBufPos = 0;

    TIM_Cmd(SND_PWM_TIMER_BASE, ENABLE); 
    TIM_Cmd(SND_SAMPLE_TIMER_BASE, ENABLE);

    return;
}

void SND_SAMPLE_IRQ_HANDLER(void)
{
    if (TIM_GetITStatus(SND_SAMPLE_TIMER_BASE, TIM_IT_Update) != RESET)
    {	 
        TIM_ClearITPendingBit(SND_SAMPLE_TIMER_BASE, TIM_IT_Update); 
        if (g_soundBufPos < sizeof(g_soundBuf)){
            TIM_SetCompare3(SND_PWM_TIMER_BASE, g_soundBuf[g_soundBufPos++]);
        } else {
            TIM_Cmd(SND_PWM_TIMER_BASE, DISABLE); 
            TIM_Cmd(SND_SAMPLE_TIMER_BASE, DISABLE); 
            //TODO 确认接口关闭的时候引脚处于低电平
        }
    }
}

