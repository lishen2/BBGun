#ifndef _BUZZER_H_
#define _BUZZER_H_

enum {
	BUZ_SOUND_BEGIN,  //game begin
	BUZ_SOUND_DEAD,   //dead
	BUZ_SOUND_EMPTY,  //ammo empty
	BUZ_SOUND_HIT,    //has been hit
	BUZ_SOUND_FIRE,   //fire
};

/* beep specific sound */
void buz_beep(int type);
					
/* init buzzer */
void buz_init(void);

#endif

