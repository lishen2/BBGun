#ifndef _BUTTON_H_
#define _BUTTON_H_

enum{
	BUTTON_NOKEY,
	BUTTON_FIRE,
	BUTTON_FUNC,
};

void button_init(void);

int button_get(void);

#endif
