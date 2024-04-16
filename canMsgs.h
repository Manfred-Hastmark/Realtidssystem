#ifndef CANMSG
#define CANMSG

#include "board_handler.h"
#include "canTinyTimber.h"

typedef unsigned char uint8_t;

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
#define HEARTBEATID 10
#define HEARTBEATTO MSEC(40)
#define HEARTBEATDL MSEC(41)
typedef struct
{
    uint8_t id;
    enum NodeState role;

} HeartBeat;

void data_to_heart_beat(CANMsg* msg, HeartBeat* heart_beat);
void heart_beat_to_data(CANMsg* msg, HeartBeat* heart_beat);

//////////////////////////////////////////////////////////////////////
///
/// @brief Claim conductor
/// id 20
///
//////////////////////////////////////////////////////////////////////
#define CLAIMCONDUCTORID 20
typedef struct
{
    uint8_t id;

} ClaimConductor;

//////////////////////////////////////////////////////////////////////
///
/// @brief Notes
///
//////////////////////////////////////////////////////////////////////
#define NOTESID 29
typedef struct
{
    uint8_t id;
    uint8_t note_index;
    char key;
    uint8_t player;
    uint8_t tempo;
} Notes;

void data_to_notes(CANMsg* msg, Notes* notes);
void notes_to_data(CANMsg* msg, Notes* notes);

//////////////////////////////////////////////////////////////////////
///
/// @brief Note acks
///
//////////////////////////////////////////////////////////////////////
#define NOTEACKSID 30
typedef struct
{
    uint8_t id;
    uint8_t index;
} NoteAcks;

//////////////////////////////////////////////////////////////////////
///
/// @brief HandoutConductor
///
//////////////////////////////////////////////////////////////////////
#define HANDOUTCONDUCTORID 40
typedef struct
{
    uint8_t id;
    uint8_t conductorId;

} HandoutConductor;

void data_to_handout_conductor(CANMsg* msg, HandoutConductor* handoutConductor);
void handout_conductor_to_data(CANMsg* msg, HandoutConductor* handoutConductor);

#endif // !CANMSGendif // !CANMSG
