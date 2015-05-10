#include "stm32f10x.h"
#include "public.h"
#include "ringbuf.h"

//check position is in range
static inline int _address_round(int size, int pos)
{
	if (pos >= size){
		pos -= size;
	}

	return pos;
}

//increase position
static inline int _address_increase(int size, int pos)
{
	return _address_round(size, ++pos);
}

void ringbuf_init(struct ringbuf_struct *buf, unsigned char* databuf, int bufsize)
{
	buf->readpos = 0;
	buf->writepos = 0;
	buf->size = bufsize;
	buf->data = databuf;
    
	return;
}

/* must assure that only one writer at one time */
int ringbuf_putchar(struct ringbuf_struct *buf, unsigned char ch)
{
	int writepos = buf->writepos;

	if (buf->readpos != _address_round(buf->size, writepos + 1)){
		buf->data[writepos] = ch;
		buf->writepos = _address_increase(buf->size, writepos);

		return ERR_OK;
	}

	return ERR_FAILED;
}

/* must assure that only one reader at one time */
int ringbuf_getchar(struct ringbuf_struct *buf, unsigned char *ch)
{
	int readpos = buf->readpos;

	if (readpos != buf->writepos){
		*ch = buf->data[readpos];
		buf->readpos = _address_increase(buf->size, readpos);;

        return ERR_OK;
	}

	return ERR_FAILED;
}

int ringbuf_getcount(struct ringbuf_struct *buf)
{
	int size;

	if (buf->writepos > buf->readpos){
		size = buf->writepos - buf->readpos;
	} else if (buf->writepos < buf->readpos) {
		size = buf->writepos + buf->size - buf->readpos;
	} else {
		size = 0;
	}

	return size;
}

