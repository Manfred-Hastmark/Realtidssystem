
#ifndef CAN_HANLDER
#define CAN_HANLDER

#include "TinyTimber.h"
#include "board_handler.h"
#include "canTinyTimber.h"
#include "part2.h"

typedef struct
{
    Object super;
    MusicPlayer* m_music_player_p;
    BoardHandler* m_board_handler_p;
    Can* m_can_p;
} CanHandler;

typedef struct
{
    int index;
    uint8_t* data;
    int length;

} CanData;

#define init_can_object(music_player_p, board_handler_p)                                                                                   \
    {                                                                                                                                      \
        initObject(), music_player_p, board_handler_p                                                                                      \
    }

/**
 * @brief Function that gets called on CAN msg recv
 */
void init_canhandler(CanHandler* self, Can* can0_p);

/**
 * @brief Function that gets called on CAN msg recv
 */
void receive_msg(CanHandler* self, uint8_t* data);

/**
 * @brief Checks timeout on all msgs
 */
void check_timeout(CanHandler* self, int id);
#endif
