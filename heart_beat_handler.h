
#ifndef HEART_BEAT_HANDLER_H
#define HEART_BEAT_HANDLER_H

#include "TinyTimber.h"
#include "application.h"
#include "board_handler.h"

typedef struct
{
    Object obj;
    App* m_app_p;
    BoardHandler* m_board_handler_p;
} HeartBeatHandler;

#define initHeartBeatHandler(app_p, board_handler_p)                                                                                       \
    {                                                                                                                                      \
        initObject(), app_p, board_handler_p                                                                                               \
    }

/**
 * @brief Starts sending the heartbeat signal
 */
void init(HeartBeatHandler* self, int unused);

/**
 * @brief Handles when a heartbeat is received
 */
void heart_beat_received(HeartBeatHandler* self, int raw_msg_p);

#endif
