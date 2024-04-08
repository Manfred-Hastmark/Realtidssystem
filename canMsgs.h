
#ifndef CANMSG
#define CANMSG

enum Role
{
    Conductor,
    Musician
};

typedef unsigned char uint8_t;

//////////////////////////////////////////////////////////////////////
///
/// @brief TimeSync
///
/// Period 100 ms
/// (Set baseline to 0)
///
//////////////////////////////////////////////////////////////////////
#define TIMESYNCID 0
typedef struct
{
    uint8_t id;

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
#define HEARTBEATID 10
typedef struct
{
    uint8_t id;
    enum Role role;

} HeartBeat;

void data_to_heart_beat(const uint8_t* data, HeartBeat* heart_beat);
void heart_beat_to_data(uint8_t* data, HeartBeat* heart_beat);

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
    uint8_t volume;

} Notes;

void data_to_notes(const uint8_t* data, Notes* notes);
void notes_to_data(uint8_t* data, Notes* notes);

//////////////////////////////////////////////////////////////////////
///
/// @brief Note acks
///
//////////////////////////////////////////////////////////////////////
#define NOTEACKSID 30
typedef struct
{
    uint8_t id;

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

void data_to_handout_conductor(uint8_t* data, HandoutConductor* handoutConductor);
void handout_conductor_to_data(uint8_t* data, HandoutConductor* handoutConductor);

#endif // !CANMSG
