
#ifndef CONDUCTOR_HANDLER
#define CONDUCTOR_HANDLER

#include "TinyTimber.h"
#include "board_handler.h"
#include "part2.h"

typedef struct
{
    Object obj;
    MusicPlayer* m_musicplayer_p;
    BoardHandler* m_board_handler_p;
} ConductorHandler;

/**
 * @brief Starts the conductor
 */
void start_conductor(ConductorHandler* self, int unused);

#endif
