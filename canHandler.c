

#include "canHandler.h"
#include "TinyTimber.h"
#include "board_handler.h"
#include "canMsgs.h"
#include "canTinyTimber.h"
#include "software_defines.h"

#define HEARTBEAT_PERIOD MSEC(100)
#define HEARTBEAT_TMO MSEC(200)

Time timeouts[MAX_NODES]{};

void init_canhandler(CanHandler* self, Can* can0_p)
{
    self->m_can0_p = can0_p;
    INSTALL(&can0_p, can_interrupt, CAN_IRQ0);
}

void notes_handler(CanHandler* self, Notes* msg);

void send_msg(CanHandler* self, int can_data_p)
{
    CANMsg msg;
    CanData* data_p = (CanData*)can_data_p;
    msg.msgId = data_p->index;
    msg.nodeId = RANK_SELF;
    msg.length = data_p->length;
    *(unsigned long long*)msg.buff = *(unsigned long long*)data_p->data;
}

void receive_msg(CanHandler* self, uint8_t* data)
{
    CANMsg msg;
    CAN_RECEIVE(self->m_can0_p, &msg);
    print("Can ID: %c, ", msg.msgId);
    uchar data_buff[8];
    *(unsigned long long*)data_buff = *(unsigned long long*)data;

    switch (msg.msgId)
    {
    case HEARTBEATID ... HEARTBEATID + MAX_NODES - 1: {
        HeartBeat heart_beat;
        data_to_heart_beat(msg.buff, &heart_beat);
        heart_beat.id = msg.msgId - HEARTBEATID;

        timeouts[heart_beat.id] = CURRENT_OFFSET();
        SetBoardState state{heart_beat.role, heart_beat.id};
        SYNC(self->m_board_handler_p, set_index, (int)&state);

        SEND(HEARTBEAT_PERIOD, HEARTBEAT_PERIOD + MSEC(1), self, check_timeout, msg.msgId - HEARTBEATID);

        break;
    }
    case CLAIMCONDUCTORID:
        return;
    case NOTESID: {
        Notes notes_msg;
        data_to_notes(data_buff, &notes_msg);
        notes_handler(self, &notes_msg);
        return;
    }
    case NOTEACKSID:
        return;
    case HANDOUTCONDUCTORID:
        return;
    default:
        return;
    }
}

void notes_handler(CanHandler* self, Notes* msg)
{
    if (msg->player == RANK_SELF)
    {
        SYNC(self->m_music_player_p->m_melody_p, setKey, msg->key);
        int melodyPeriods[LENGTH];
        SYNC(self->m_music_player_p->m_melody_p, setMelodyPeriods, (int)melodyPeriods);
        SYNC(self->m_music_player_p->m_melody_p, setPeriods, (int)melodyPeriods);
        SYNC(&self->m_music_player_p->TG, volume, msg->volume);
        SYNC(&self->m_music_player_p->TG, set_note_index, msg->note_index);
        ASYNC(self->m_music_player_p, nextBeat, 0);
    }
}

void check_timeout(CanHandler* self, int id)
{
    if (CURRENT_OFFSET() - timeouts[id] > HEARTBEAT_TMO)
    {
        SetBoardState state{DISCONNECTED, id};
        SYNC(self->m_board_handler_p, set_index, (int)&state);
    }
}
