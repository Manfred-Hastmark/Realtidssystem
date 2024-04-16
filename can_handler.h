
#ifndef CAN_HANDLER_H
#define CAN_HANDLER_H

#include "TinyTimber.h"
#include "application.h"
#include "canTinyTimber.h"
#include "heart_beat_handler.h"
#include "part2.h"

typedef struct
{
    Object obj;
    App* m_app_p;
    HeartBeatHandler* m_heart_beat_handler_p;
    MusicPlayer* m_music_player_p;
} CanHandler;

#define initCanHandler(app_p, heart_beat_handler_p, music_player_p)                                                                        \
    {                                                                                                                                      \
        initObject(), app_p, heart_beat_handler_p, music_player_p                                                                          \
    }

/**
 * @brief Handles received can messages
 */
void can_receiver(CanHandler* self, int msg_p);

#endif
