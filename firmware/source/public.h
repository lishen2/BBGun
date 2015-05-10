#ifndef _PUBLIC_H_
#define _PUBLIC_H_

#define ERR_OK     0
#define ERR_FAILED -1

#define BOOL_TRUE  1
#define BOOL_FALSE 0

#define inline __inline

#ifdef DEBUG
#define ASSERT(exp)\
	if (!(exp)) assert_failed(__FILE__, __LINE__);
#else
#define ASSERT(exp)
#endif

/* is 'a' latter than 'b' */
#define time_after(a,b) ((s32)((s32)(b) - (s32)(a)) < 0)

#define HZ 100

extern vu32 g_jiffies;

void system_init(void);
void delay_ms(int ms);
void assert_failed(uint8_t* file, uint32_t line);

#endif
