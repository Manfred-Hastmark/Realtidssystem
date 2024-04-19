#ifndef APPLICATION_H
#define APPLICATION_H

#include "TinyTimber.h"
#include "canTinyTimber.h"

#define MAX_BUFFER 32
#define UNUSED 0
#define BURST_INTERVAL MSEC(500)

typedef struct
{
    Object super;
    CANMsg buffer[MAX_BUFFER];
    int msg_to_handle;
    int msg_recv;
    int app_ready;
    int inter_arrival;
} Regulator;


typedef struct
{
    Object super;
    int burst_mode;
    int msg_id;
    int print_out;
} Transmitter;

typedef struct
{
    Object super;
    char buffer[50]; // Maximum amount of characters to input at once will be 50
    int index;
} ReadBuffer;





#define initRegulator() {initObject(), {}, 0, 0, 1, 1}
#define initTransmitter() {initObject(), 0, 0, 0}
#define initReadBuffer() { initObject(), {}, 0 }


void print(char*, int);
void set_ready(Regulator* self, int unused);
void handle_msg(Regulator* self, int unused);
int buffer_full(Regulator* self, int unused);

void reader(Regulator*, int);
void receive(Regulator*, int);
void send(Transmitter*, int);
void keyHandler(Regulator*, int);

void readBufferAdd(ReadBuffer*, int);
int endBuffer(ReadBuffer*, int);


#endif
