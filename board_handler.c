
#include "board_handler.h"
#include "application.h"
#include "canMsgs.h"

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
