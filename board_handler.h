
#ifndef BOARD_HANDLER
#define BOARD_HANDLER

#include "TinyTimber.h"

#define MAX_NODES 8

typedef struct
{
    Object obj;
    int nodes[MAX_NODES];
    int number_of_nodes;
} BoardHandler;

#define init_board_handler()                                                                                                               \
    {                                                                                                                                      \
        initObject(), {0, 0, 0, 0, 0, 0, 0, 0}, 0                                                                                          \
    }

/**
 * @brief Scans CAN to find all connected boards
 */
void initialize(BoardHandler* self, int unused);

/**
 * @brief Gets the next index to of board to play
 */
int get_next_index(BoardHandler* self, int unused);

#endif
