#include "canMsgs.h"

void data_to_heart_beat(CANMsg* msg, HeartBeat* heart_beat)
{
    heart_beat->id = msg->msgId;
    heart_beat->role = msg->buff[0];
}

void heart_beat_to_data(CANMsg* msg, HeartBeat* heart_beat)
{
    // Set all bytes to 0
    *(unsigned long long*)msg->buff = 0;
    msg->buff[0] = heart_beat->role;
    msg->length = 1;
    msg->msgId = heart_beat->id;
}

void data_to_notes(CANMsg* msg, Notes* notes)
{
    notes->id = msg->msgId;
    notes->note_index = msg->buff[0];
    notes->key = *(char*)msg->buff[1];
    notes->player = msg->buff[2];
    notes->tempo = msg->buff[3];
    notes->volume = msg->buff[4];
}

void notes_to_data(CANMsg* msg, Notes* notes)
{
    // Set all bytes to 0
    *(unsigned long long*)msg->buff = 0;

    msg->buff[0] = notes->note_index;
    msg->buff[1] = *(uint8_t*)notes->key;
    msg->buff[2] = notes->player;
    msg->buff[3] = notes->tempo;
    msg->buff[4] = notes->volume;
    msg->length = 5;
    msg->msgId = notes->id;
}

void data_to_handout_conductor(CANMsg* msg, HandoutConductor* handoutConductor)
{
    handoutConductor->id = msg->msgId;
    handoutConductor->conductorId = msg->buff[0];
}

void handout_conductor_to_data(CANMsg* msg, HandoutConductor* handoutConductor)
{
    *(unsigned long long*)msg->buff = 0;
    msg->buff[0] = handoutConductor->conductorId;
    msg->msgId = handoutConductor->id;
    msg->length = 1;
}
