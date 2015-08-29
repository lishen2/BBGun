#ifndef _USART_IO_H_
#define _USART_IO_H_

/*
* send one character through USART using interrupt mode
* just write character to buffer and return
* @param void* port          USART peripheral regs base address, eg. USART1
*/
int usartio_sendchar(void* port, unsigned char ch);

/* 
* send one character through USART using polling mode
* when function return, the sending is finish
*/
int usartio_sendchar_polling(void* port, unsigned char ch);

/* 
* read one character from receive buffer
*/
int usartio_recvchar(void* port, unsigned char* ch);

/* read data count from receive buffer */
int usartio_receive_count(void *port);

void usartio_init_usart1(void);
void usartio_init_usart2(void);
void usartio_init_usart3(void);

#endif

