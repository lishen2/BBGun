#ifndef _IR_H_
#define _IR_H_

/***********************************************
* IR System
***********************************************/

#define IR_BYTE1_ADD_HEALTH    0x80
#define IR_BYTE1_ADD_ROUNDS    0x81
#define IR_BYTE1_COMMAND       0x83
#define IR_BYTE1_SYSTEM_DATA   0x87
#define IR_BYTE1_CLIPS_PICKUP  0x8A
#define IR_BYTE1_HEALTH_PICKUP 0x8B
#define IR_BYTE1_FLAG_PICKUP   0x8C

//availably when byte1 is IR_BYTE1_COMMAND
#define IR_BYTE2_COMMAND_ADMIN_KILL        0x00	//Admin Kill
#define IR_BYTE2_COMMAND_PAUSE             0x01	//Pause/Unpause
#define IR_BYTE2_COMMAND_START_GAME        0x02	//Start Game
#define IR_BYTE2_COMMAND_RESTORE_DEFAULT   0x03	//Restore Defaults
#define IR_BYTE2_COMMAND_RESPAWN           0x04	//Respawn
#define IR_BYTE2_COMMAND_NEW_GAME          0x05	//New Game (Immediate)
#define IR_BYTE2_COMMAND_FULL_AMMO         0x06	//Full Ammo
#define IR_BYTE2_COMMAND_END_GAME          0x07	//End Game
#define IR_BYTE2_COMMAND_RESET_CLOCK       0x08	//Reset Clock
#define IR_BYTE2_COMMAND_INITIALIZE_PLAYER 0x0A	//Initialize Player
#define IR_BYTE2_COMMAND_EXPLODE_PLAYER    0x0B	//Explode Player
#define IR_BYTE2_COMMAND_NEW_GAME          0x0C	//New Game (Ready)
#define IR_BYTE2_COMMAND_FULL_HEALTH       0x0D	//Full Health
#define IR_BYTE2_COMMAND_FULL_ARMOR        0X0F	//Full Armor
#define IR_BYTE2_COMMAND_CLARE_SCORES      0x14	//Clear Scores
#define IR_BYTE2_COMMAND_TEST_SENSORS      0x15	//Test Sensors
#define IR_BYTE2_COMMAND_STUN_PLAYER       0x16	//Stun Player
#define IR_BYTE2_COMMAND_DISARM_PLAYER     0x17	//Disarm Player   

//availably when byte1 is IR_BYTE1_SYSTEM_DATA
#define IR_BYTE2_SYSTEM_CLONE_DATA  0x01
#define IR_BYTE2_SYSTEM_SCORE_DATA1 0x03
#define IR_BYTE2_SYSTEM_SCORE_DATA2 0x04
#define IR_BYTE2_SYSTEM_SCORE_DATA3 0x05

/**
* fire function
* @param PID player ID  1~127
* @param TID team ID    0~3
* @param damage         0~100
* note: parameter 'damage' is not literal
0000 = 1
0001 = 2
0010 = 4
0011 = 5
0100 = 7
0101 = 10
0110 = 15
0111 = 17
1000 = 20
1001 = 25
1010 = 30
1011 = 35
1100 = 40
1101 = 50
1110 = 75
1111 = 100
*/
void ir_send_shot(uint8_t player_id, uint8_t team_id, uint8_t damage);

/* send message */
void ir_send_message(uint8_t byte1, uint8_t byte2, uint8_t *extra, uint8_t ext_len);

/* 
* handle received message
*/
void ir_handle_msg(void);

void ir_init(void);

#endif

