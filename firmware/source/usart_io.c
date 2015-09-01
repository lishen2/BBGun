#include <stdio.h>
#include <string.h>
#include "stm32f10x.h"
#include "ringbuf.h"
#include "usart_io.h"
#include "public.h"

#define USARTIO_FLAG_RXNE  0x20
#define USARTIO_FLAG_TXE   0x80

#define USARTIO_IsFlagSet(USART_ADDR, FLAG) (0 != (USART_ADDR->SR & FLAG))

//USART1 used to print debug information and firmware upgrade
#define USARTIO_USART1_GPIO       GPIOA
#define USARTIO_USART1_GPIO_CLK   RCC_APB2Periph_GPIOA
#define USARTIO_USART1_RxPin      GPIO_Pin_10
#define USARTIO_USART1_TxPin      GPIO_Pin_9
#define USARTIO_USART1_CLK        RCC_APB2Periph_USART1
#define USARTIO_USART1_IRQ        USART1_IRQn

//USART2 and USART3 RX pin connect to two IR receive ICs,
//USART3 TX pin connect IR emitter diode
#define USARTIO_USART2_GPIO      GPIOA
#define USARTIO_USART2_GPIO_CLK  RCC_APB2Periph_GPIOA
#define USARTIO_USART2_RxPin     GPIO_Pin_3
#define USARTIO_USART2_CLK       RCC_APB1Periph_USART2
#define USARTIO_USART2_IRQ       USART2_IRQn

#define USARTIO_USART3_GPIO      GPIOB
#define USARTIO_USART3_GPIO_CLK  RCC_APB2Periph_GPIOB
#define USARTIO_USART3_RxPin     GPIO_Pin_11
#define USARTIO_USART3_TxPin     GPIO_Pin_10
#define USARTIO_USART3_CLK       RCC_APB1Periph_USART3
#define USARTIO_USART3_IRQ       USART3_IRQn

/* USART1 send and receive buffer size */
#define USARTIO_U1_READBUF_SIZE  64 
#define USARTIO_U1_WRITEBUF_SIZE 1024

/* USART2 is receive only, USART3 use polling mode to send, so they both not use send buffer */
#define USARTIO_U2_READBUF_SIZE  256 
#define USARTIO_U3_READBUF_SIZE  256 

static unsigned char g_usart1_readbuf[USARTIO_U1_READBUF_SIZE]; 
static unsigned char g_usart1_writebuf[USARTIO_U1_WRITEBUF_SIZE]; 
struct ringbuf_struct g_Usart1ReadBufHead = {
    0,
    0,
    sizeof(g_usart1_readbuf),
    g_usart1_readbuf,
};
struct ringbuf_struct g_Usart1WriteBufHead = {
    0,
    0,
    sizeof(g_usart1_writebuf),
    g_usart1_writebuf,
};

static unsigned char g_usart2_readbuf[USARTIO_U2_READBUF_SIZE]; 
struct ringbuf_struct g_Usart2ReadBufHead = {
    0,
    0,
    sizeof(g_usart2_readbuf),
    g_usart2_readbuf,
};

static unsigned char g_usart3_readbuf[USARTIO_U3_READBUF_SIZE];  
struct ringbuf_struct g_Usart3ReadBufHead = {
    0,
    0,
    sizeof(g_usart3_readbuf),
    g_usart3_readbuf,
};

static USART_InitTypeDef USART_InitStructure;

int usartio_sendchar(void* port, unsigned char ch)
{  
    int ret;

	//usart1 use interrupt mode
    if (USART1 == port){
	    ret = ringbuf_putchar(&g_Usart1WriteBufHead, ch);
	    USART_ITConfig(port, USART_IT_TXE, ENABLE);
    } 
	else {
        return ERR_FAILED;
    }

    return ret;
}

int usartio_sendchar_polling(void* port, unsigned char ch)
{  
	//usart3 use polling mode
	if (USART3 == port){
	    USART_SendData(port, ch);
	    while(USART_GetFlagStatus(port, USART_FLAG_TXE) == RESET);
    } else {
        return ERR_FAILED;
    }

    return ERR_OK;
}

int usartio_recvchar(void *port, unsigned char *ch, int timeout)
{
    int ret;
    struct ringbuf_struct *readBuf;

    if (USART1 == port){
        readBuf = &g_Usart1ReadBufHead;
    } else if (USART2 == port){
        readBuf = &g_Usart2ReadBufHead;
    } else if (USART3 == port){
        readBuf = &g_Usart3ReadBufHead;
    } else {
        return ERR_FAILED;
    }

    timeout += g_jiffies;
    while (1){
        ret = ringbuf_getchar(readBuf, ch);
        if (ERR_OK == ret || time_after(g_jiffies, timeout)){
            break;
        }
    }

    return ret;
}

int usartio_receive_count(void *port)
{
	struct ringbuf_struct *readBuf;

    if (USART1 == port){
        readBuf = &g_Usart1ReadBufHead;
    } else if (USART2 == port){
        readBuf = &g_Usart2ReadBufHead;
    } else if (USART3 == port){
        readBuf = &g_Usart3ReadBufHead;
    } else {
        return 0;
    }

	return ringbuf_getcount(readBuf);
}

void usartio_init_usart1(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;    

    RCC_APB2PeriphClockCmd(USARTIO_USART1_GPIO_CLK, ENABLE);
    RCC_APB2PeriphClockCmd(USARTIO_USART1_CLK, ENABLE);

    /* Enable the USART Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USARTIO_USART1_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* GPIO */
    /* Configure Rx as input floating */
    GPIO_InitStructure.GPIO_Pin = USARTIO_USART1_RxPin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(USARTIO_USART1_GPIO, &GPIO_InitStructure);

    /* Configure Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = USARTIO_USART1_TxPin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(USARTIO_USART1_GPIO, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStructure);

    /* enable interrupt */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);

    USART_Cmd(USART1, ENABLE);

    return;
}

void usartio_init_usart2(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;  

    RCC_APB2PeriphClockCmd(USARTIO_USART2_GPIO_CLK, ENABLE);  
    RCC_APB1PeriphClockCmd(USARTIO_USART2_CLK, ENABLE);

    /* Enable the debug USART Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USARTIO_USART2_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* GPIO */
    /* Configure debug USART Rx as input floating */
    GPIO_InitStructure.GPIO_Pin = USARTIO_USART2_RxPin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(USARTIO_USART2_GPIO, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 300; //根据所选接收管的限制，计算得出最大值为712
    USART_InitStructure.USART_WordLength = USART_WordLength_8b; 
    USART_InitStructure.USART_StopBits = USART_StopBits_1;      
    USART_InitStructure.USART_Parity = USART_Parity_No;         
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx;
    USART_Init(USART2, &USART_InitStructure);

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART2, ENABLE);

    /* Set the USARTy prescaler */
    USART_SetPrescaler(USART2, 0x1);
    /* Configure the USARTy IrDA mode */
    USART_IrDAConfig(USART2, USART_IrDAMode_Normal);
    
    /* Enable the USARTy IrDA mode */
    USART_IrDACmd(USART2, ENABLE);

    return;
}

void usartio_init_usart3(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;  

    RCC_APB2PeriphClockCmd(USARTIO_USART3_GPIO_CLK, ENABLE);  
    RCC_APB1PeriphClockCmd(USARTIO_USART3_CLK, ENABLE);

    /* Enable the debug USART Interrupt */
    NVIC_InitStructure.NVIC_IRQChannel = USARTIO_USART3_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    /* GPIO */
    /* Configure debug USART Rx as input floating */
    GPIO_InitStructure.GPIO_Pin = USARTIO_USART3_RxPin;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(USARTIO_USART3_GPIO, &GPIO_InitStructure);

    /* Configure debug USART Tx as alternate function push-pull */
    GPIO_InitStructure.GPIO_Pin = USARTIO_USART3_TxPin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(USARTIO_USART3_GPIO, &GPIO_InitStructure);

    USART_InitStructure.USART_BaudRate = 300; //根据所选接收管的限制，计算得出最大值为712
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;     
    USART_InitStructure.USART_Parity = USART_Parity_No;        
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART3, &USART_InitStructure);

    USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART3, ENABLE);

    /* Set the USARTy prescaler */
    USART_SetPrescaler(USART3, 0x1);
    /* Configure the USARTy IrDA mode */
    USART_IrDAConfig(USART3, USART_IrDAMode_Normal);
    
    /* Enable the USARTy IrDA mode */
    USART_IrDACmd(USART3, ENABLE);

    return;
}
/*
void usartio_disable_usart3_tx(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = USARTIO_USART3_TxPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(USARTIO_USART3_GPIO, &GPIO_InitStructure);	 

	return;	
}

void usartio_enable_usart3_tx(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = USARTIO_USART3_TxPin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(USARTIO_USART3_GPIO, &GPIO_InitStructure);

	return;
}
*/
/**
  * @brief  This function handles USART1 global interrupt request.
  * @param  None
  * @retval None
  */
void USART1_IRQHandler(void)
{
  int ret;
  unsigned char ch;
  
  if(USARTIO_IsFlagSet(USART1, USARTIO_FLAG_RXNE) == SET)
  {
    /* read data from reg and put into buffer */
    ringbuf_putchar(&g_Usart1ReadBufHead, 
                    (unsigned char)USART_ReceiveData(USART1));
  }
  
  if(USARTIO_IsFlagSet(USART1, USARTIO_FLAG_TXE) == SET)
  { 
    /* read data from buffer and put into reg */
    ret = ringbuf_getchar(&g_Usart1WriteBufHead, &ch);
    if (ERR_OK == ret){
        USART_SendData(USART1, (unsigned short)ch);  
    } 
    /* if buffer empty disable inttrupt */
    else {
        USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
    }//else
  }//if
}

/**
  * @brief  This function handles USART2 global interrupt request.
  * @param  None
  * @retval None
  */
void USART2_IRQHandler(void)
{
  if(USARTIO_IsFlagSet(USART2, USARTIO_FLAG_RXNE) == SET)
  {
    /* read data from reg and put into buffer */
    ringbuf_putchar(&g_Usart2ReadBufHead, 
                    (unsigned char)USART_ReceiveData(USART2));
  }
}

/**
  * @brief  This function handles USART3 global interrupt request.
  * @param  None
  * @retval None
  */
void USART3_IRQHandler(void)
{
  if(USARTIO_IsFlagSet(USART3, USARTIO_FLAG_RXNE) == SET)
  {
    /* read data from reg and put into buffer */
    ringbuf_putchar(&g_Usart3ReadBufHead, 
                    (unsigned char)USART_ReceiveData(USART3));
  }
}

