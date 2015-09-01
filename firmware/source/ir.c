#include "stm32f10x.h"
#include "public.h"
#include "usart_io.h"
#include "ir.h"

#define IR_FIRE_USART   USART3
#define IR_RECV_UASRT   USART2 

//max wait time when recving system data message
#define IR_SYSTEM_DATA_RECV_TIMEOUT 3000

//38KHZ PWM pin
#define IR_PWM_PIN         GPIO_Pin_6
#define IR_PWM_PORT        GPIOA 
#define IR_PWM_PORT_CLOCK  RCC_APB2Periph_GPIOA
#define IR_PWM_TIMER       TIM3
#define IR_PWM_TIMER_CLOCK RCC_APB1Periph_TIM3

enum{
    IR_STATE_START,
    IR_STATE_SHOT,
    IR_STATE_MESSAGE,
};

static int g_ir_state;
static unsigned char g_buf[128];
static unsigned char g_bufpos;

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

static void _ir_handle_system_data(void)
{
    int i;
    int ret;
    uint8_t data;
    
    switch(g_buf[1])
    {
    case 0x01:
        for (i = 0; i < ; ++i){
            ret = usartio_recvchar(IR_RECV_UASRT, &data, IR_SYSTEM_DATA_RECV_TIMEOUT);
            if (ERR_OK != ret){
                break;
            }
        }
        if (sizeof(struct logic_clone_data) == i){
            logic_clone_data(g_buf + 3);
        }
        break;
    case 0x03:
        break;
    case 0x04:
        break;
    case 0x05:
        break;
    default:
        break;
    };

    return;
}

/* 
* handle received message
*/
void ir_handle_msg(void)
{
    uint8_t data;
    int ret;
    
    while(1){
        ret = usartio_recvchar(IR_RECV_UASRT, &data, 0);
        if (ERR_OK != ret){
            break;
        }

        g_buf[g_bufpos++] = data;
        
        switch(g_ir_state)
        {
        case IR_STATE_START:  
            if (0 == 0x80 & data) {
                g_ir_state = IR_STATE_SHOT;
            } else {
                g_ir_state = IR_STATE_MESSAGE;
            }
            break;
        case IR_STATE_SHOT:
            //shot package has only two bytes 
            if (2 == g_bufpos){
                logic_got_shot(g_buf[0] & 0x7F,
                                  (g_buf[1] >> 6) & 0x03,
                                  (g_buf[1] >> 2) & 0x0F);

                g_bufpos = 0;
                g_ir_state = IR_STATE_START;
            }
            break;
        case IR_STATE_MESSAGE:
            //message package generally has three bytes, expect for 'System Data'
            if (3 == g_bufpos){
                switch(g_buf[0]){
                case 0x80:
                    logic_add_health(g_buf[1]);
                    break;
                case 0x81;
                    logic_add_rounds(g_buf[1]);
                    break;
                case 0x83:
                    logic_handle_command(g_buf[1]);
                    break;
                case 0x87:
                    _ir_handle_system_data(void);
                    break;
                case 0x8A:
                    logic_clips_pickup(g_buf[1]);
                    break;
                case 0x8B:
                    logic_health_pickup(g_buf[1]);
                    break;
                case 0x8C:
                    logic_flag_pickup(g_buf[1]);
                    break;
                default:
                    break;
                };

                g_bufpos = 0;
                g_ir_state = IR_STATE_START;                
            }
            break;            
        };
    }

    return;
}

void ir_init(void)
{
	_gpio_init();
	_config_pwm();

    memset(g_buf, 0, sizeof(g_buf));
    g_bufpos = 0;
    g_ir_state = IR_STATE_START;

	return;
}

void ir_send_shot(uint8_t player_id, uint8_t team_id, uint8_t damage)
{
    uint8_t buf[2];

    buf[0] = player_id & 0x7F;
    buf[1] = ((team_id & 0x03) << 6) | ((damage & 0x0F) << 2);

	usartio_sendchar_polling(IR_FIRE_USART, buf[0]);
	usartio_sendchar_polling(IR_FIRE_USART, buf[1]);

    return;
}

void ir_send_message(uint8_t byte1, uint8_t byte2, uint8_t *extra, uint8_t ext_len)
{
    int i;
    
    usartio_sendchar_polling(byte1);
    usartio_sendchar_polling(byte2);
    usartio_sendchar_polling(0xE8);
    
    for (i = 0; i < ext_len; ++i){
        usartio_sendchar_polling(extra[i]);
    }

    return;
}

