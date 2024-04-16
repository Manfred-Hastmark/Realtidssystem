

#include "canHandler.h"
#include "TinyTimber.h"
#include "application.h"
#include "board_handler.h"
#include "canMsgs.h"
#include "canTinyTimber.h"
#include "part2.h"
#include <stdio.h>

#define HEARTBEAT_PERIOD MSEC(100)
#define HEARTBEAT_TMO MSEC(200)

void notes_handler(MusicPlayer* self, Notes* msg);

unsigned short timeouts[MAX_NODES] = {0, 0, 0, 0, 0, 0, 0, 0};

void init_canhandler(CanHandler* self, Can* can0_p)
{
    self->m_can_p = can0_p;
}

void receive_msg(CanHandler* self, uint8_t* data)
{
    CANMsg msg;
    CAN_RECEIVE(self->m_can_p, &msg);

    switch (msg.msgId)
    {
    case HEARTBEATID:
    case HEARTBEATID + 1:
    case HEARTBEATID + 2:
    case HEARTBEATID + 3:
    case HEARTBEATID + 4:
    case HEARTBEATID + 5:
    case HEARTBEATID + 6:
    case HEARTBEATID + 7: {
        HeartBeat heart_beat;
        data_to_heart_beat(&msg, &heart_beat);
        heart_beat.id -= HEARTBEATID;

        SetBoardState state = {heart_beat.role, heart_beat.id};
        SYNC(self->m_board_handler_p, set_index, (int)&state);

        timeouts[heart_beat.id]++;
        unsigned int data = (timeouts[heart_beat.id] << 16) + (heart_beat.id & 0xFFFF);

        SEND(HEARTBEAT_TMO, HEARTBEAT_TMO + USEC(1), self, check_timeout, data);
        return;
    }
    break;
    case CLAIMCONDUCTORID:
    case CLAIMCONDUCTORID + 1:
    case CLAIMCONDUCTORID + 2:
    case CLAIMCONDUCTORID + 3:
    case CLAIMCONDUCTORID + 4:
    case CLAIMCONDUCTORID + 5:
    case CLAIMCONDUCTORID + 6:
    case CLAIMCONDUCTORID + 7: {
        if (SYNC(self->m_board_handler_p, is_conductor, 0) == 1)
        {
            // Print claim conductor
            ASYNC(self->m_board_handler_p, handout_conductor, CLAIMCONDUCTORID - MAX_NODES);
        }
        return;
    }
    case NOTESID: {
        Notes notes_msg;
        data_to_notes(&msg, &notes_msg);
        notes_handler(self->m_music_player_p, &notes_msg);
        return;
    }
    break;
    case NOTEACKSID:
    case NOTEACKSID + 1:
    case NOTEACKSID + 2:
    case NOTEACKSID + 3:
    case NOTEACKSID + 4:
    case NOTEACKSID + 5:
    case NOTEACKSID + 6:
    case NOTEACKSID + 7: {

        ASYNC(self->m_music_player_p, notes_ack, msg.buff[0]);
        return;
    }

    case HANDOUTCONDUCTORID:
    case HANDOUTCONDUCTORID + 1:
    case HANDOUTCONDUCTORID + 2:
    case HANDOUTCONDUCTORID + 3:
    case HANDOUTCONDUCTORID + 4:
    case HANDOUTCONDUCTORID + 5:
    case HANDOUTCONDUCTORID + 6:
    case HANDOUTCONDUCTORID + 7: {
        HandoutConductor handout_conductor_msg;
        data_to_handout_conductor(&msg, &handout_conductor_msg);
        self->m_board_handler_p->node_states[handout_conductor_msg.id - HANDOUTCONDUCTORID] = MUSICIAN;

        SetBoardState state = {CONDUCTOR, handout_conductor_msg.conductorId};
        SYNC(self->m_board_handler_p, set_index, (int)&state);

        if (SYNC(self->m_board_handler_p, is_conductor, 0) == 1)
        {
            ASYNC(self->m_music_player_p, nextBeat, 0);
        }
        return;
    }
    break;
    default:
        return;
    }
}

void notes_handler(MusicPlayer* self, Notes* msg)
{
    // SYNC(&self->TG, set_note_index, msg->note_index);
    if (msg->player == RANK)
    {
        // SYNC(self->m_melody_p, setKey, msg->key);
        // int melodyPeriods[LENGTH];
        // SYNC(self->m_melody_p, setMelodyPeriods, (int)melodyPeriods);
        // SYNC(self->m_melody_p, setPeriods, (int)melodyPeriods);
        //
        // self->TG.silence = 0;
        // self->TG.period = self->notePeriods[self->index];
        // ASYNC(&self->TG, setDAC, 0xFFFFFFFF);

        send_ack(self, msg->note_index);
    }
}

void check_timeout(CanHandler* self, int id)
{
    unsigned short to_id = ((unsigned int)id) >> 16;
    unsigned short msg_id = ((unsigned int)id) & 0xFFFF;
    if (timeouts[msg_id] == to_id)
    {
        SetBoardState state = {DISCONNECTED, msg_id};
        SYNC(self->m_board_handler_p, set_index, (int)&state);
    }
}
