
#ifndef BOARD_HANDLER_H
#define BOARD_HANDLER_H

#include "TinyTimber.h"
#include "application.h"

#define DEFAULT_REQUEST_INDEX 100

enum NodeState
{
    CONDUCTOR = 0,
    MUSICIAN = 1,
    DISCONNECTED = 2,
};

typedef struct
{
    Object obj;
    App* m_app_p;
    enum NodeState node_states[MAX_BOARDS];
    int nodes_connected;
    int conductor_change;
    int new_conductor_index;
    int request_ongoing;
    int request_index;
} BoardHandler;

#define initBoardHandler(app_p)                                                                                                            \
    {                                                                                                                                      \
        initObject(), app_p,                                                                                                               \
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
            0, 0, 0, 0, DEFAULT_REQUEST_INDEX                                                                                              \
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

/**
 * @brief Request to claim conductorship
 */
int request_conductorship(BoardHandler* self, int unused);

/**
 * @brief Updates board state with who is conductor
 */
void handle_conductorship_handout(BoardHandler* self, int index);

/**
 * @brief return nr of board
 */
int number_of_boards(BoardHandler* self, int unused);

/**
 * @brief Handle conductorship request
 */
void handle_conductorship_request(BoardHandler* self, int index);

/**
 * @brief print status of all boards
 */
void print_status(BoardHandler* self, int unused);

/**
 * @brief Checks if the network has a conductor and if we should step up
 */
void check_stepup(BoardHandler* self, int unused);

/**
 * @brief Updates the the index of the lowest RANK requesting CONDUCTOR
 */
void lowest_request_index(BoardHandler* self, int index);

#endif
