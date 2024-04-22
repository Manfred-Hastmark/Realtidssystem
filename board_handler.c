
#include "board_handler.h"
#include "TinyTimber.h"
#include "application.h"
#include "canMsgs.h"

#define CLAIM_WAIT_TIME MSEC(1000)

void commit_claim_request(BoardHandler* self, int unused);

void handle_node_timeout(BoardHandler* self, int index)
{
    print("Node with id %i disconnected\n", index);
    if (self->node_states[index] != DISCONNECTED)
    {
        self->nodes_connected--;
    }
    self->node_states[index] = DISCONNECTED;

    if (self->node_states[RANK] != DISCONNECTED)
    {
        ASYNC(self, check_stepup, 0);
    }
}

void handle_node_alive(BoardHandler* self, int raw_heart_beat_msg_p)
{
    if (self->node_states[RANK] != DISCONNECTED)
    {
        ASYNC(self, check_stepup, 0);
    }

    HeartBeat* heart_beat_msg_p = (HeartBeat*)raw_heart_beat_msg_p;
    if (self->node_states[heart_beat_msg_p->id] == DISCONNECTED)
    {
        if (self->nodes_connected == 1 && self->node_states[RANK] == MUSICIAN)
        {
            print("I Now Join As A Musician\n", 0);
        }
        self->nodes_connected++;
    }
    self->node_states[heart_beat_msg_p->id] = heart_beat_msg_p->role;
}

int get_next_player(BoardHandler* self, int unused)
{
    static int current_player;
    for (int i = 0; i < MAX_BOARDS; i++)
    {
        current_player++;
        current_player %= MAX_BOARDS;
        if (self->node_states[current_player] != DISCONNECTED)
        {
            return current_player;
        }
    }
    return -1;
}

int has_conductor(BoardHandler* self, int unused)
{
    for (int i = 0; i < MAX_BOARDS; i++)
    {
        if (self->node_states[i] == CONDUCTOR)
        {
            return 1;
        }
    }
    return 0;
}

int request_conductorship(BoardHandler* self, int unused)
{
    ASYNC(self->m_app_p, send_claim_conductorship, unused);
}

void handle_conductorship_handout(BoardHandler* self, int index)
{
    if (index < 0 || index > MAX_BOARDS - 1)
    {
        return;
    }
    self->node_states[index] = CONDUCTOR;
    if (index == RANK)
    {
        ASYNC(self->m_app_p, start_playing, 0);
    }
}

void handle_conductorship_request(BoardHandler* self, int index)
{
    if (index < self->request_index)
    {
        self->request_index = index;
    }
    if (!self->request_ongoing)
    {
        self->request_ongoing = 1;
        AFTER(CLAIM_WAIT_TIME, self, commit_claim_request, 0);
    }
}

void commit_claim_request(BoardHandler* self, int unused)
{
    self->node_states[RANK] = MUSICIAN;
    self->request_ongoing = 0;
    self->conductor_change = 1;
    self->new_conductor_index = self->request_index;
    self->request_index = DEFAULT_REQUEST_INDEX;
}

void print_status(BoardHandler* self, int unused)
{
    print("Status:\n", 0);
    for (int i = 0; i < MAX_BOARDS; i++)
    {
        print("%i ", self->node_states[i]);
    }
    print("\n", 0);
}

int number_of_boards(BoardHandler* self, int unused)
{
    int boards_connected = 0;
    for (int i = 0; i < MAX_BOARDS; i++)
    {
        if (self->node_states[i] != DISCONNECTED)
        {
            boards_connected++;
        }
    }
    return boards_connected;
}

void check_stepup(BoardHandler* self, int unused)
{
    int num_boards = 0;
    for (int i = 0; i < MAX_BOARDS; i++)
    {
        if (self->node_states[i] == CONDUCTOR)
        {
            return; // We won't step up, since a conductor is already present
        }
        if (self->node_states[i] == MUSICIAN)
        {
            num_boards++;
        }
    }

    if (num_boards > 1) // If there are more than 1 musicians and no conductors a step-up should be made
    {
        if (RANK < self->request_index)
        {
            self->request_index = RANK;
        }
        AFTER(CLAIM_WAIT_TIME, self, set_conductor, 0); // Lowest rank will get conductor
    }
}

void set_conductor(BoardHandler* self, int unused)
{
    self->node_states[self->request_index] = CONDUCTOR;
    if (RANK == self->request_index)
    {
        ASYNC(self->m_app_p, start_playing, 0);
        print("I Am The New Conductor\n", 0);
    }
    self->request_index = DEFAULT_REQUEST_INDEX;
}

void lowest_request_index(BoardHandler* self, int index)
{
    if (index < self->request_index)
    {
        self->request_index = index;
    }
}
