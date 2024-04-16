

#include "canHandler.h"
#include "TinyTimber.h"
#include "application.h"
#include "canMsgs.h"
#include "canTinyTimber.h"
#include "software_defines.h"

Time last_heart_beat_1 = 0;
Time last_heart_beat_2 = 0;

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
    case HEARTBEATID + RANK_OTHER_1:
        last_heart_beat_1 = CURRENT_OFFSET();
        SEND(HEARTBEATTO, HEARTBEATDL, &self->app, heartbeat_tmo_check_1, (int)&last_heart_beat_1);
        return;
    case HEARTBEATID + RANK_OTHER_2:
        last_heart_beat_2 = CURRENT_OFFSET();
        SEND(HEARTBEATTO, HEARTBEATDL, &self->app, heartbeat_tmo_check_2, (int)&last_heart_beat_2);
        return;
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
        //SYNC(self->m_music_player_p->m_melody_p, setKey, msg->key);
        int melodyPeriods[LENGTH];
        //SYNC(self->m_music_player_p->m_melody_p, setMelodyPeriods, (int)melodyPeriods);
        //SYNC(self->m_music_player_p->m_melody_p, setPeriods, (int)melodyPeriods);
        SYNC(&self->m_music_player_p->TG, volume, msg->volume);
        SYNC(&self->m_music_player_p->TG, set_index, msg->note_index);
        ASYNC(self->m_music_player_p, nextBeat, 0);
    }
}
