#ifndef _SOUND_H_
#define _SOUND_H_

enum {
    SND_SHOT           = 0,
    SND_EMPTY_CHAMBER,
    SND_START_RELOAD,
    SND_END_RELOAD,
    SND_NEAR_MISS,
    SND_HIT_DAMAGE,
    SND_DEAD,
    SND_POWER_UP,
    SND_BEEP,
    SND_BUZZ,
    SND_PLUS_MEDIC,
    SND_PLUS_AMMO,
    SND_GAME_OVER,
    SND_EXPLOSION,
    SND_END,
};

void sound_init(void);
void sound_play(int effect);

void sound_update(int effect, uint8_t *sound);

#endif

