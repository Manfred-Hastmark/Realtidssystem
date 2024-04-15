
#ifndef APPLICATION
#define APPLICATION

#define LENGTH 32

#include "TinyTimber.h"
#include "canMsgs.h"

typedef struct
{
    Object super;
    int count;
    char c;
} App;

static App app = {initObject(), 0, 'X'};

void heartbeat_tmo_check_1(App* self, int call_time_p);

void heartbeat_tmo_check_2(App* self, int call_time_p);

#endif
