#ifndef _LOGIC_H_
#define _LOGIC_H_

struct logic_clone_data
{
    uint8_t start_mark1;  //0x08
    uint8_t start_mark2;  //0xE8
    uint8_t start_mark3;  //0x21
    uint8_t team_id;
    uint8_t __r1;
    uint8_t ammo_box;
    uint8_t medic_box;
    uint8_t __r2;
    uint8_t led_timeout;
    uint8_t sounds;
    uint8_t overheat_limit;
    uint8_t __r3;
    uint8_t __r4;
    uint8_t damage;
    uint8_t clip_size;
    uint8_t clips;
    uint8_t fire_select;
    uint8_t burst_rounds;
    uint8_t cyclic_rate;
    uint8_t reload_delay;
    uint8_t ir_power;
    uint8_t ir_range[2];
    uint8_t health;
    uint8_t __r5;
    uint8_t respawn_delay;
    uint8_t armor[3];
    uint8_t hit_delay;
    uint8_t start_delay;
    uint8_t death_delay;
    uint8_t time_limit;
    uint8_t respawns;
    uint8_t __r6;
    uint8_t checksum;
};

struct logic_core_data
{
    //persion
    uint8_t team_id;
    uint8_t player_id;
    uint8_t armor;     //armor absorb half of the damage
    uint8_t health;

    //gun
    uint8_t name[8];
    uint8_t damage;    
    uint8_t clip_size;
    uint8_t clips;
    uint8_t cur_clip;
    uint8_t reload_delay; //delay when doing reload
    uint8_t shot_delay;   //delay between every shot

    //game
    uint8_t time_limit;   //total game limit
};

//called by ir
void logic_add_health(uint8_t value);
void logic_add_rounds(uint8_t value);
void logic_handle_command(uint8_t cmd);
void logic_clone_data(struct logic_clone_data *data);
void logic_clips_pickup(uint8_t id);
void logic_health_pickup(uint8_t id);
void logic_flag_pickup(uint8_t id);
void logic_got_shot(uint8_t player_id, uint8_t team_id, uint8_t damage);

//called by menu
struct logic_core_data* logic_get_core_data(void);

//called by main
void logic_handle_key(void);

#endif

