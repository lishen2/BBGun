#ifndef _IR_H_
#define _IR_H_

/***********************************************
* IR System
***********************************************/

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
void ir_send_shot(unsigned char PID, unsigned char TID, unsigned char damage);

/* send add health message
@param health 1 ~ 100
*/
void ir_send_add_health(unsigned char health);

/* send command
0x00	Admin Kill
0x01	Pause/Unpause
0x02	Start Game
0x03	Restore Defaults
0x04	Respawn
0x05	New Game (Immediate)
0x06	Full Ammo
0x07	End Game
0x08	Reset Clock
0x09	RESERVED
0x0A	Initialize Player
0x0B	Explode Player
0x0C	New Game (Ready)
0x0D	Full Health
0x0E	RESERVED
0X0F	Full Armor
0x10	RESERVED
0x11	RESERVED
0x12	RESERVED
0x13	RESERVED
0x14	Clear Scores
0x15	Test Sensors
0x16	Stun Player
0x17	Disarm Player
*/
void ir_send_command(unsigned char command);

/* clone sys*/
void ir_send_clone(unsigned char* data, int datasize);

/* 
* handle received message
*/
void ir_handle_msg(void);

void ir_init(void);

#endif

