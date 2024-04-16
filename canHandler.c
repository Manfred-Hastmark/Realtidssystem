

#include "canHandler.h"
#include "TinyTimber.h"
#include "board_handler.h"
#include "canMsgs.h"
#include "canTinyTimber.h"
#include "part2.h"
#include "software_defines.h"
#include <stdio.h>

#define HEARTBEAT_PERIOD MSEC(100)
#define HEARTBEAT_TMO MSEC(200)

void notes_handler(CanHandler* self, Notes* msg);

Time timeouts[MAX_NODES] = {0, 0, 0, 0, 0, 0, 0, 0};

void init_canhandler(CanHandler* self, Can* can0_p)
{
    self->m_can_p = can0_p;
}

void receive_msg(CanHandler* self, uint8_t* data)
{
    print("Received can msg %i\n", 1);

    CANMsg msg;
    CAN_RECEIVE(self->m_can_p, &msg);
    print("Can ID: %c, ", msg.msgId);
    uchar data_buff[8];
    *(unsigned long long*)data_buff = *(unsigned long long*)data;

    switch (msg.msgId)
    {
    case HEARTBEATID ... HEARTBEATID + MAX_NODES - 1: {
        HeartBeat heart_beat;
        data_to_heart_beat(msg.buff, &heart_beat);
        heart_beat.id -= HEARTBEATID;

        timeouts[heart_beat.id] = CURRENT_OFFSET();
        SetBoardState state = {heart_beat.role, heart_beat.id};
        SYNC(self->m_board_handler_p, set_index, (int)&state);

        SEND(HEARTBEAT_PERIOD, HEARTBEAT_PERIOD + MSEC(1), self, check_timeout, heart_beat.id);
        break;
    }
    case CLAIMCONDUCTORID ... CLAIMCONDUCTORID + MAX_NODES - 1:

        if (SYNC(self->m_board_handler_p, is_conductor, 0) == 1)
        {
            // Print claim conductor
            ASYNC(self->m_board_handler_p, handout_conductor, CLAIMCONDUCTORID - MAX_NODES);
        }

        return;
    case NOTESID: {
        print("Received notes msg %i\n", 1);
        Notes notes_msg;
        data_to_notes(data_buff, &notes_msg);
        notes_handler(self, &notes_msg);
        return;
    }
    case NOTEACKSID ... NOTEACKSID + MAX_NODES - 1: {
        Notes notes_msg;
        data_to_notes(data_buff, &notes_msg);
        ASYNC(self->m_music_player_p, notes_ack, notes_msg.note_index);
        return;
    }
    case HANDOUTCONDUCTORID ... HANDOUTCONDUCTORID + MAX_NODES - 1: {
        HandoutConductor handout_conductor_msg;
        data_to_handout_conductor(&msg, &handout_conductor_msg);
        self->m_board_handler_p->node_states[handout_conductor_msg.id - HANDOUTCONDUCTORID] = MUSICIAN;

        SetBoardState state = {CONDUCTOR, handout_conductor_msg.conductorId};
        SYNC(self->m_board_handler_p, set_index, (int)&state);

        if (SYNC(self->m_board_handler_p, is_conductor, 0) == 1)
        {
            ASYNC(self->m_music_player_p, nextBeat, 0);
        }
    }
        return;
    default:
        return;
    }
}

void notes_handler(CanHandler* self, Notes* msg)
{
    SYNC(&self->m_music_player_p->TG, set_note_index, msg->note_index);
    if (msg->player == RANK_SELF)
    {
        SYNC(self->m_music_player_p->m_melody_p, setKey, msg->key);
        int melodyPeriods[LENGTH];
        SYNC(self->m_music_player_p->m_melody_p, setMelodyPeriods, (int)melodyPeriods);
        SYNC(self->m_music_player_p->m_melody_p, setPeriods, (int)melodyPeriods);
        self->m_music_player_p->TG.silence = 0;
        self->m_music_player_p->TG.period = self->m_music_player_p->notePeriods[self->m_music_player_p->index];
        ASYNC(&self->m_music_player_p->TG, setDAC, 0xFFFFFFFF);
    }
}

void check_timeout(CanHandler* self, int id)
{
    if (CURRENT_OFFSET() - timeouts[id] > HEARTBEAT_TMO)
    {
        SetBoardState state = {DISCONNECTED, id};
        SYNC(self->m_board_handler_p, set_index, (int)&state);
    }
}
