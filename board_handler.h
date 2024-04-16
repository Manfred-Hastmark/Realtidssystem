
#ifndef BOARD_HANDLER
#define BOARD_HANDLER

#include "TinyTimber.h"
#include "canMsgs.h"

#define MAX_NODES 8
#define LENGTH 32

typedef struct
{
    Object obj;
    enum BoardState node_states[MAX_NODES];
    int number_of_nodes;
} BoardHandler;

#define init_board_handler()                                                                                                               \
    {                                                                                                                                      \
        initObject(), {0, 0, 0, 0, 0, 0, 0, 0}, 0                                                                                          \
    }

typedef struct
{
    enum BoardState state;
    int index;
} SetBoardState;


/**
 * @brief Scans CAN to find all connected boards
 */
void initialize(BoardHandler* self, int unused);

/**
 * @brief Gets if board is conductor
 */
int is_conductor(BoardHandler* self, int unused);


/**
 * @brief Gets the next musician index to play
 */
int get_next_musician_index(BoardHandler* self, int unused);

/**
 * @brief Gets the index of the current conductor
 */
int get_conductor_index(BoardHandler* self, int unused);

/**
 * @brief Sets a certain board to certain status.
 *          Takes SetBoardState as arg
 */
void set_index(BoardHandler* self, int set);

/**
 * @brief Returns the lowest player id
 */
int get_lowest_id(BoardHandler* self, int unused);

/**
 * @brief Handles timeouts and conductor ship
 */
void update_behaviour(BoardHandler* self, int unused);

void handout_conductor(BoardHandler* self, int unused);
#endif
