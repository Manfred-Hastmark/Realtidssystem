#include "canMsgs.h"

void data_to_heart_beat(const uint8_t* data, HeartBeat* heart_beat)
{
    heart_beat->id = HEARTBEATID;
    heart_beat->role = data[0];
}

void heart_beat_to_data(uint8_t* data, HeartBeat* heart_beat)
{
    // Set all bytes to 0
    *(unsigned long long*)data = 0;
    data[0] = heart_beat->role;
}

void data_to_notes(const uint8_t* data, Notes* notes)
{
    notes->id = NOTESID;
    notes->note_index = data[0];
    notes->key = *(char*)data[1];
    notes->player = data[2];
    notes->tempo = data[3];
    notes->volume = data[4];
}

void notes_to_data(uint8_t* data, Notes* notes)
{
    // Set all bytes to 0
    *(unsigned long long*)data = 0;

    data[0] = notes->note_index;
    data[1] = *(uint8_t*)notes->key;
    data[2] = notes->player;
    data[3] = notes->tempo;
    data[4] = notes->volume;
}

void data_to_handout_conductor(uint8_t* data, HandoutConductor* handoutConductor)
{
    handoutConductor->id = HANDOUTCONDUCTORID;
    handoutConductor->conductorId = data[0];
}

void handout_conductor_to_data(uint8_t* data, HandoutConductor* handoutConductor)
{
    *(unsigned long long*)data = 0;
    data[0] = handoutConductor->conductorId;
}
