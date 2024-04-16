
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

void heartbeat_tmo_check_1(App* self, int call_time_p);

void heartbeat_tmo_check_2(App* self, int call_time_p);

void send_tone_msg(MusicPlayer* musig_player_p, int notes_msg_p);

#endif
