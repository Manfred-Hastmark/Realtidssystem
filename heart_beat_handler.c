
#include "heart_beat_handler.h"
#include "TinyTimber.h"
#include "application.h"
#include "board_handler.h"
#include "canMsgs.h"

#define HEARTBEAT_TO MSEC(210)

typedef struct
{
    unsigned short timeout;
    unsigned short id;
} TOContainer;

int hb_timeouts[MAX_BOARDS];

void heart_beat(HeartBeatHandler* self, int unused);
void handle_timeout(HeartBeatHandler* self, int raw_to_container);

void init(HeartBeatHandler* self, int unused)
{
    AFTER(MSEC(100), self, heart_beat, unused);
}

void heart_beat_received(HeartBeatHandler* self, int raw_msg_p)
{
    HeartBeat* msg_p = (HeartBeat*)raw_msg_p;

    hb_timeouts[msg_p->id]++;
    TOContainer to_container;
    to_container.id = msg_p->id;
    to_container.timeout = hb_timeouts[msg_p->id];
    AFTER(HEARTBEAT_TO, self, handle_timeout, *(int*)&to_container);

    ASYNC(self->m_board_handler_p, handle_node_alive, raw_msg_p);
}

void heart_beat(HeartBeatHandler* self, int unused)
{
    if (!self->m_app_p->to_heart_beat)
    {
        if (self->m_board_handler_p->node_states[RANK] == DISCONNECTED)
        {
            ASYNC(self->m_app_p, send_heart_beat, MUSICIAN);
        }
        else
        {
            ASYNC(self->m_app_p, send_heart_beat, self->m_board_handler_p->node_states[RANK]);
        }
    }
    AFTER(MSEC(100), self, heart_beat, unused);
}

void handle_timeout(HeartBeatHandler* self, int raw_to_container)
{
    TOContainer to_container = *(TOContainer*)&raw_to_container;
    if (to_container.timeout == hb_timeouts[to_container.id])
    {
        ASYNC(self->m_board_handler_p, handle_node_timeout, to_container.id);
    }
}
