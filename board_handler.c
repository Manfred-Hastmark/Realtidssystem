
#include "board_handler.h"

int get_next_musician_index(BoardHandler* self, int unused)
{
    static int current_musician_index = 0;
    for (int i = 0; i < MAX_NODES; i++)
    {
        if (self->node_states[current_musician_index] != DISCONNECTED)
        {
            return current_musician_index;
        }
        current_musician_index++;
        current_musician_index %= MAX_NODES;
    }
    return -1;
}

void set_index(BoardHandler* self, int set)
{
    SetBoardState* set_state = (SetBoardState*)set;

    if (set_state->index < 0 || set_state->index > (MAX_NODES - 1))
    {
        return;
    }

    self->node_states[set_state->index] = set_state->state;

    self->number_of_nodes = 0;
    for (int i = 0; i < MAX_NODES; i++)
    {
        if (self->node_states[i] != DISCONNECTED)
        {
            self->number_of_nodes++;
        }
    }
}
