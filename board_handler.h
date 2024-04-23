
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
    int schedule_handout;
    int connection;
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
 * @brief Request to claim conductorship
 */
void request_conductorship(BoardHandler* self, int unused);

/**
 * @brief Updates board state with who is conductor
 */
void handle_conductorship_handout(BoardHandler* self, int index);

/**
 * @brief What to do if can cable is cut
 */
int handle_conductor_disconnect(BoardHandler* self, int unused);

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

/**
 * @brief Flags for a handout at the end of the note
 */
void commit_claim_request(BoardHandler* self, int unused);

/**
 * @brief Initialize the claim window
 */
void init_claim(BoardHandler* self, int index);

/**
 * @brief set the ongoing status
 */
void set_request_ongoing(BoardHandler* self, int set);

/**
 * @brief Perform the handout
 */
void perform_handout(BoardHandler* self, int unused);

/**
 * @brief Used to cancel the reconnection fase
 */
void join_choir(BoardHandler* self, int unused);

/**
 * @brief On CAN disconnect reset state variables
 */
void reset_connection(BoardHandler* self, int unused);
#endif
