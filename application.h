
#ifndef APPLICATION
#define APPLICATION

#include "part2.h"
#define RANK 0

#include "TinyTimber.h"
#include "canMsgs.h"

typedef struct
{
    Object super;
    int count;
    char c;
} App;

void send_tone_msg(MusicPlayer* musig_player_p, int notes_msg_p);

void send_conductor_handout_msg(BoardHandler* board_handler, int handout_msg_p);

void print(char* string, int val);

#endif
