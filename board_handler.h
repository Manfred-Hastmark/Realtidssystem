
#ifndef BOARD_HANDLER_H
#define BOARD_HANDLER_H

#include "TinyTimber.h"
#include "application.h"

enum NodeState
{
    CONDUCTOR = 0,
    MUSICIAN = 1,
    DISCONNECTED = 2,
};

enum DefaultTones
{
    DEFAULT_KEY = 0,
    DEFAULT_BPM = 120,
};

typedef struct
{
    Object obj;
    enum NodeState node_states[MAX_BOARDS];
    int nodes_connected;
} BoardHandler;

#define initBoardHandler()                                                                                                                 \
    {                                                                                                                                      \
        initObject(),                                                                                                                      \
            {                                                                                                                              \
                DISCONNECTED,                                                                                                              \
                DISCONNECTED,                                                                                                              \
                DISCONNECTED,                                                                                                              \
                DISCONNECTED,                                                                                                              \
                DISCONNECTED,                                                                                                              \
                DISCONNECTED,                                                                                                              \
                DISCONNECTED,                                                                                                              \
                DISCONNECTED,                                                                                                              \
            },                                                                                                                             \
            0                                                                                                                              \
    }

/**
 * @brief Call function to handle when a node timeouts
 */
void handle_node_timeout(BoardHandler* self, int index);

/*
 * @brief Handles when a node is sending their state
 */
void handle_node_alive(BoardHandler* self, int raw_heart_beat_msg_p);

/**
 * @brief Retrieves next player to play note
 */
int get_next_player(BoardHandler* self, int unused);

/**
 * @brief Checks if there's a conductor in network
 */
int has_conductor(BoardHandler* self, int unused);

#endif
