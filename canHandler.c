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

void notes_handler(MusicPlayer* self, int msg);

unsigned short timeouts[MAX_NODES] = {0, 0, 0, 0, 0, 0, 0, 0};

void init_canhandler(CanHandler* self, Can* can0_p)
{
    self->m_can_p = can0_p;
}

void receive_msg(CanHandler* self, uint8_t* data)
{
    CANMsg msg;
    CAN_RECEIVE(self->m_can_p, &msg);

    if (msg.msgId != 10)
    {
        print("Msg rec id: %i\n", msg.msgId);
    }

    switch (msg.msgId)
    {
    case HEARTBEATID ... HEARTBEATID + MAX_NODES - 1: {
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
    case CLAIMCONDUCTORID ... CLAIMCONDUCTORID + MAX_NODES - 1: {
        if (SYNC(self->m_board_handler_p, is_conductor, 0) == 1)
        {
            ASYNC(self->m_board_handler_p, handout_conductor, CLAIMCONDUCTORID - MAX_NODES);
        }
        return;
    }
    case NOTESID: {
        Notes notes_msg;
        data_to_notes(&msg, &notes_msg);
        SYNC(self->m_music_player_p, notes_handler, (int)&notes_msg);
        return;
    }
    case NOTEACKSID ... NOTEACKSID + MAX_NODES - 1: {

        ASYNC(self->m_music_player_p, notes_ack, msg.buff[0]);
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
        return;
    }
    default:
        return;
    }
}

void notes_handler(MusicPlayer* self, int msg)
{
    Notes* notes = (Notes*)msg;
    SYNC(&self->TG, set_note_index, notes->note_index);
    if (notes->player == RANK)
    {
        SYNC(self->m_melody_p, setKey, notes->key);
        int melodyPeriods[LENGTH];
        SYNC(self->m_melody_p, setMelodyPeriods, (int)melodyPeriods);
        SYNC(self, setPeriods, (int)melodyPeriods);

        self->TG.silence = 0;
        self->TG.period = self->notePeriods[self->index];
        SEND(MSEC(1), MSEC(2), &self->TG, setDAC, 0xFFFFFFFF);

        ASYNC(self, send_ack, notes->note_index);
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
