#ifndef _RANGBUF_H_
#define _RANGBUF_H_

struct ringbuf_struct{
	volatile int readpos;
	volatile int writepos;	
	unsigned int size;
    unsigned char* data; 
};

/* determine whether buffer is empty */
#define RINGBUF_ISEMPTY(buf) ((buf)->readpos == (buf)->writepos)

/* init rangbuffer structer, with buffer and buffer size */
void ringbuf_init(struct ringbuf_struct *buf, unsigned char* databuf, int bufsize);

/* put one char into buf, 
 * return ERROR_OK on success, 
 * otherwise return ERR_FAILED
 * */
int ringbuf_putchar(struct ringbuf_struct *buf, unsigned char ch);

/* get one char from buf, 
 * return ERROR_OK on success, 
 * otherwise return ERR_FAILED
 * */
int ringbuf_getchar(struct ringbuf_struct *buf, unsigned char *ch);

/* read data count */
int ringbuf_getcount(struct ringbuf_struct *buf);

#endif

