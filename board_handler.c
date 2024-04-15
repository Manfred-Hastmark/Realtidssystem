
#include "board_handler.h"

int current_index = 0;

int get_next_index(BoardHandler* self, int unused)
{
    for (int i = 0; i < MAX_NODES; i++)
    {
        if (self->nodes[current_index] != 0)
        {
            return current_index;
        }
        current_index++;
        current_index %= MAX_NODES;
    }
    return -1;
}
