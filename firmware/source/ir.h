#ifndef _IR_H_
#define _IR_H_

/***********************************************
* IR System
***********************************************/
//²½Ç¹
#define IR_FIRE_RUFKE(team, persion)\
	ir_fire(team, persion, (unsigned char)20, (unsigned char)2000)
//¾Ñ»÷Ç¹
#define IR_FIRE_SNIPE(team, persion)\
	ir_fire(team, persion, (unsigned char)45, (unsigned char)5000)
//»úÇ¹
#define IR_FIRE_MACHINE(team, persion)\
	ir_fire(team, persion, (unsigned char)10, (unsigned char)200)

/**
* fire function
* @param firetime how long this shoot take
*/
void ir_fire(unsigned char team, unsigned char persion, unsigned char damage, unsigned char firetime);

/**
* are we been hit?
*/
int ir_ishit(unsigned char *team, unsigned char *persion, unsigned char *damage);

void ir_init(void);

#endif

