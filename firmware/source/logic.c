#include "public.h"
#include "ir.h"
#include "logic.h"

struct logic_core_data
{
    //persion
    uint8_t team_id;
    uint8_t player_id;
    uint8_t armor;
    uint8_t health;

    //gun
    uint8_t fire_select;
    uint8_t damage;
    uint8_t clip_size;
    uint8_t clips;
    uint8_t reload_delay;

    //game
    uint8_t respawn_delay;
    uint8_t respawns;
    uint8_t time_limit;
};

