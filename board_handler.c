
#include "board_handler.h"
#include "TinyTimber.h"
#include "application.h"
#include "canMsgs.h"

#define CLAIM_WAIT_TIME MSEC(1000)
#define REQUEST_INDEX_DEFAULT 100

int request_index = REQUEST_INDEX_DEFAULT;
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
}

void handle_node_alive(BoardHandler* self, int raw_heart_beat_msg_p)
{
    HeartBeat* heart_beat_msg_p = (HeartBeat*)raw_heart_beat_msg_p;
    if (self->node_states[heart_beat_msg_p->id] == DISCONNECTED)
    {
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

void request_conductorship(BoardHandler* self, int unused)
{
    if (!SYNC(self, has_conductor, 0))
    {
        self->node_states[RANK] = CONDUCTOR;
        return;
    }
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
    if (index < request_index)
    {
        request_index = index;
    }
    if (!request_ongoing)
    {
        request_ongoing = 1;
        AFTER(CLAIM_WAIT_TIME, self, commit_claim_request, 0);
    }
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

void commit_claim_request(BoardHandler* self, int unused)
{
    self->node_states[RANK] = MUSICIAN;
    self->conductor_change = 1;
    self->new_conductor_index = request_index;
    request_ongoing = 0;
    request_index = REQUEST_INDEX_DEFAULT;
}
