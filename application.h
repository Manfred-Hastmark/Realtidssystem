#ifndef APPLICATION_H
#define APPLICATION_H

#include "TinyTimber.h"

#define RANK 0
#define LENGTH 32
#define MAX_BOARDS 8

typedef struct
{
    Object super;
    int count;
    char c;
    int to_heart_beat;
} App;

void print(char*, int);

/**
 * @brief Sends a heartbeat signal on can
 */
void send_heart_beat(App* self, int role);

#endif
