
#include "board_handler.h"
#include "TinyTimber.h"
#include "application.h"
#include "canMsgs.h"

#define CLAIM_WAIT_TIME MSEC(1000)

int request_index;
int request_ongoing;

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
    request_index = index;
    if (!request_ongoing)
    {
        request_ongoing = 1;
        AFTER(CLAIM_WAIT_TIME, self, commit_claim_request, 0);
    }
}

void commit_claim_request(BoardHandler* self, int unused)
{
    self->node_states[RANK] = MUSICIAN;
    request_ongoing = 0;
    self->conductor_change = 1;
    self->new_conductor_index = request_index;
}

void print_status(BoardHandler* self, int unused)
{
    print("Status:\n", 0);
    for (int i = 0; i < MAX_BOARDS; i++)
    {
        print("%i ", self->node_states[i]);
    }
    print("\n ", 0);
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
}

void check_stepup(BoardHandler* self, int unused)
{
    int boards_connected = 0;
    int conductor = 0;
    int lowest_id = 10;

    for (int i = 0; i < MAX_BOARDS; i++)
    {
        if (self->node_states[i] == CONDUCTOR)
        {
            conductor = 1;
        }

        if (self->node_states[i] != DISCONNECTED)
        {
            if (lowest_id > i)
            {
                lowest_id = i;
            }
            boards_connected++;
        }
    }

    if (conductor == 0 && lowest_id == RANK && boards_connected != 1)
    {
        self->node_states[RANK] = CONDUCTOR;
        ASYNC(self->m_app_p, start_playing, 0);
        print("Am The New Conductor\n", 0);
    }
}
