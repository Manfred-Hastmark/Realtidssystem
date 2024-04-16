
#ifndef CAN_HANLDER
#define CAN_HANLDER

#include "TinyTimber.h"
#include "application.h"
#include "board_handler.h"
#include "canMsgs.h"
#include "canTinyTimber.h"
#include "part2.h"

typedef struct
{
    Object super;
    App* app;
    MusicPlayer* m_music_player_p;
    BoardHandler* m_board_handler_p;
    Can* m_can0_p;
} CanHandler;

typedef struct
{
    int index;
    uint8_t* data;
    int length;

} CanData;

#define init_can_object(app, music_player_p, board_handler_p)                                                                                  \
    {                                                                                                                                      \
        initObject(), app, music_player_p, board_handler_p                                                                                      \
    }

/**
 * @brief Function that gets called on CAN msg recv
 */
void init_canhandler(CanHandler* self, Can* can0_p);

/**
 * @brief Function that gets called on CAN msg recv
 */
void send_msg(CanHandler* self, int can_data_p);

/**
 * @brief Function that gets called on CAN msg recv
 */
void receive_msg(CanHandler* self, uint8_t* data);

/**
 * @brief Checks timeout on all msgs
 */
void check_timeout();

#endif
