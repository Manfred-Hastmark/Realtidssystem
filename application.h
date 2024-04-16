#ifndef APPLICATION_H
#define APPLICATION_H

#include "TinyTimber.h"

#define RANK 1
#define LENGTH 32
#define MAX_BOARDS 8

typedef struct
{
    Object super;
    int count;
    char c;
    int to_heart_beat;
    int ack_notes;
} App;

void print(char*, int);

/**
 * @brief Sends a heartbeat signal on can
 */
void send_heart_beat(App* self, int role);

/**
 * @brief Send a notes can message
 */
void send_notes_msg(App* self, int raw_notes_msg_p);

/**
 * @brief Sends note ack message
 */
void send_note_ack(App* self, int note_index);

#endif
