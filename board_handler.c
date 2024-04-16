
#include "board_handler.h"
#include "TinyTimber.h"
#include "application.h"
#include "canMsgs.h"
#include "canTinyTimber.h"

int is_conductor(BoardHandler* self, int unused)
{
    return self->node_states[RANK] == CONDUCTOR;
}

int get_next_musician_index(BoardHandler* self, int unused)
{
    int iter = 0;
    self->board_to_play = (self->board_to_play + 1) % MAX_NODES;
    while(self->node_states[self->board_to_play] == DISCONNECTED){
        self->board_to_play = (self->board_to_play + 1) % MAX_NODES;
        iter++;
        if(iter >= MAX_NODES){
            return -1;
        }
    }
    for(int i = 0; i < MAX_NODES; i++){
        print("%i, ", self->node_states[i]);
    }
    print("\n", 0);
    return self->board_to_play;
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

    print("index: %i, ", set_state->index);
    print("state: %i \n", set_state->state);
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
    // for (int i = 0; i < MAX_NODES; i++)
    // {
    //     if (self->node_states[i] != DISCONNECTED)
    //     {
    //         return i;
    //     }
    // }
    // return -1;
}

void update_behaviour(BoardHandler* self, int unused)
{
    // static int last_nr_boards = 0;
    // // Wait for init
    // if (self->node_states[RANK] == DISCONNECTED)
    // {
    //     return;
    // }
    //
    // // We are solo
    // if (self->number_of_nodes == 1 && last_nr_boards > 2)
    // {
    //     self->node_states[RANK] = MUSICIAN;
    // }
    //
    // // Assign new conductor
    // if (SYNC(self, get_conductor_index, 0) == -1 && self->number_of_nodes > 1)
    // {
    //     int lowest_rank_id = SYNC(self, get_lowest_id, 0);
    //     if (lowest_rank_id != -1)
    //     {
    //         if (lowest_rank_id == RANK)
    //         {
    //             self->node_states[RANK] = CONDUCTOR;
    //         }
    //     }
    // }
    //
    // last_nr_boards = self->number_of_nodes;
}

#define CLAIM_DURATION MSEC(3000)
int lowest_id = -1;

void handout_conductor(BoardHandler* self, int id)
{
    // if (lowest_id == -1)
    // {
    //     SEND(CLAIM_DURATION, CLAIM_DURATION + MSEC(1), self, send_handout_msg, 0);
    //     lowest_id = id;
    // }
    // else
    // {
    //     if (id < lowest_id)
    //     {
    //         lowest_id = id;
    //     }
    // }
}

void send_handout_msg(BoardHandler* self, int unused)
{
    // self->node_states[RANK] = MUSICIAN;
    // self->node_states[lowest_id] = CONDUCTOR;
    //
    // HandoutConductor handout_conductor_msg;
    // handout_conductor_msg.id = HANDOUTCONDUCTORID + RANK;
    // handout_conductor_msg.conductorId = lowest_id;
    //
    // CANMsg msg;
    // handout_conductor_to_data(&msg, &handout_conductor_msg);
    //
    // lowest_id = -1;
    // SYNC(self, send_handout_msg, (int)&msg);
    // // send msg
}
