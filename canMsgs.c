#include "canMsgs.h"

void data_to_heart_beat(CANMsg* msg, HeartBeat* heart_beat)
{
    heart_beat->id = msg->msgId;
    heart_beat->role = msg->buff[0];
}

void heart_beat_to_data(CANMsg* msg, HeartBeat* heart_beat)
{
    // Set all bytes to 0
    for (int i = 0; i < 8; i++)
    {
        msg->buff[i] = 0;
    }
    msg->buff[0] = heart_beat->role;
    msg->msgId = heart_beat->id;
    msg->length = 1;
}

void data_to_notes(CANMsg* msg, Notes* notes)
{
    notes->id = msg->msgId;
    notes->note_index = msg->buff[0];
    notes->key = (signed char)msg->buff[1];
    notes->player = msg->buff[2];
    notes->tempo = msg->buff[3];
}

void notes_to_data(CANMsg* msg, Notes* notes)
{
    for (int i = 0; i < 8; i++)
    {
        msg->buff[i] = 0;
    }

    msg->buff[0] = notes->note_index;
    msg->buff[1] = (unsigned char)notes->key;
    msg->buff[2] = notes->player;
    msg->buff[3] = notes->tempo;
    msg->length = 4;
    msg->msgId = notes->id;
}

void data_to_handout_conductor(CANMsg* msg, HandoutConductor* handoutConductor)
{
    handoutConductor->id = msg->msgId;
    handoutConductor->conductorId = msg->buff[0];
}

void handout_conductor_to_data(CANMsg* msg, HandoutConductor* handoutConductor)
{
    for (int i = 0; i < 8; i++)
    {
        msg->buff[i] = 0;
    }
    msg->buff[0] = handoutConductor->conductorId;
    msg->msgId = handoutConductor->id;
    msg->length = 1;
}
