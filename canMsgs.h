
#ifndef CANMSG
#define CANMSG
#include "enums.h"

typedef unsigned char uint8_t;

//////////////////////////////////////////////////////////////////////
///
/// @brief TimeSync
///
/// Period 100 ms
/// (Set baseline to 0)
///
//////////////////////////////////////////////////////////////////////
typedef struct
{
    uint8_t id{0};

} TimeSync;

//////////////////////////////////////////////////////////////////////
///
/// @brief Heart beat
///
/// Period 20 ms
/// Timeout 40 ms
/// id: 10
/// id: 11
/// id: 12
///
//////////////////////////////////////////////////////////////////////
typedef struct
{
    uint8_t id{10};
    Role role{Role::Musician};

} HeartBeat;

inline void data_to_heart_beat(const uint8_t* data, HeartBeat* heart_beat)
{
    heart_beat->role = (Role)data[0];
}

inline void heart_beat_to_data(uint8_t* data, HeartBeat* heart_beat)
{
    // Set all bytes to 0
    *(unsigned long long*)data = 0;
    data[0] = heart_beat->role;
}

//////////////////////////////////////////////////////////////////////
///
/// @brief Claim conductor
///
//////////////////////////////////////////////////////////////////////
typedef struct
{
    uint8_t id{20};

} ClaimConductor;

//////////////////////////////////////////////////////////////////////
///
/// @brief Notes
///
//////////////////////////////////////////////////////////////////////
typedef struct
{
    uint8_t id{29};
    uint8_t note_index{0};
    char key{0};
    uint8_t player{0};
    uint8_t tempo{0};
    uint8_t volume{0};

} Notes;

inline void data_to_notes(const uint8_t* data, Notes* notes)
{
    notes->note_index = data[0];
    notes->key = *(char*)data[1];
    notes->player = data[2];
    notes->tempo = data[3];
    notes->volume = data[4];
}

inline void notes_to_data(uint8_t* data, Notes* notes)
{
    // Set all bytes to 0
    *(unsigned long long*)data = 0;

    data[0] = notes->note_index;
    data[1] = *(uint8_t*)notes->key;
    data[2] = notes->player;
    data[3] = notes->tempo;
    data[4] = notes->volume;
}

//////////////////////////////////////////////////////////////////////
///
/// @brief Note acks
///
//////////////////////////////////////////////////////////////////////
typedef struct
{
    uint8_t id{30};

} NoteAcks;

//////////////////////////////////////////////////////////////////////
///
/// @brief HandoutConductor
///
//////////////////////////////////////////////////////////////////////
typedef struct
{
    uint8_t id{40};
    uint8_t conductorId{0};

} HandoutConductor;

inline void data_to_handout_conductor(uint8_t* data, HandoutConductor* handoutConductor)
{
    handoutConductor->conductorId = data[0];
}

inline void handout_conductor_to_data(uint8_t* data, HandoutConductor* handoutConductor)
{
    *(unsigned long long*)data = 0;
    data[0] = handoutConductor->conductorId;
}

#endif // !CANMSG
