
#include "board_handler.h"
#include "application.h"
#include "canMsgs.h"

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

int get_conductor_index(BoardHandler* self, int unused)
{
    for (int i = 0; i < MAX_NODES; i++)
    {
        if (self->node_states[i] == CONDUCTOR)
        {
            return i;
        }
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
    ASYNC(self, update_behaviour, 0);
}

int get_lowest_id(BoardHandler* self, int unused)
{
    for (int i = 0; i < MAX_NODES; i++)
    {
        if (self->node_states[i] != DISCONNECTED)
        {
            return i;
        }
    }
    return -1;
}

void update_behaviour(BoardHandler* self, int unused)
{
    // Wait for init
    if (self->node_states[RANK] == DISCONNECTED)
    {
        return;
    }

    // We are solo
    if (self->number_of_nodes == 1)
    {
        self->node_states[RANK] = MUSICIAN;
    }

    // Assign new conductor
    if (SYNC(self, get_conductor_index, 0) == -1)
    {
        int lowest_rank_id = SYNC(self, get_lowest_id, 0);
        if (lowest_rank_id != -1)
        {
            if (lowest_rank_id == RANK)
            {
                self->node_states[RANK] = CONDUCTOR;
            }
        }
    }
}
