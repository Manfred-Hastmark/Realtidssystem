
#include "can_handler.h"
#include "TinyTimber.h"
#include "application.h"
#include "board_handler.h"
#include "canMsgs.h"
#include "canTinyTimber.h"
#include "part2.h"

void can_receiver(CanHandler* self, int raw_msg_p)
{
    CANMsg* msg_p = (CANMsg*)raw_msg_p;

    switch (msg_p->msgId)
    {
    case HEARTBEATID ... HEARTBEATID + MAX_BOARDS - 1: {
        static HeartBeat heart_beat_msg;
        data_to_heart_beat(msg_p, &heart_beat_msg);
        heart_beat_msg.id -= HEARTBEATID;
        ASYNC(self->m_heart_beat_handler_p, heart_beat_received, (int)&heart_beat_msg);
        break;
    }
    case NOTESID: {
        Notes notes_msg;
        data_to_notes(msg_p, &notes_msg);
        self->m_music_player_p->index = notes_msg.note_index;
        SYNC(self->m_music_player_p, change_key, notes_msg.key);
        SYNC(self->m_music_player_p, change_bpm, notes_msg.tempo);
        if (notes_msg.player == RANK)
        {
            if (self->m_app_p->ack_notes)
            {
                ASYNC(self->m_music_player_p, play_note, notes_msg.note_index);
                ASYNC(self->m_app_p, send_note_ack, notes_msg.note_index);
            }
        }
        break;
    }
    case NOTEACKSID ... NOTEACKSID + MAX_BOARDS - 1: {
        ASYNC(self->m_music_player_p, note_ack_received, msg_p->buff[0]);
        print("Recv note ack id: %i\n", msg_p->buff[0]);
        break;
    }
    case HANDOUTCONDUCTORID ... HANDOUTCONDUCTORID + MAX_BOARDS - 1: {
        ASYNC(self->m_board_handler_p, handle_conductorship_handout, msg_p->buff[0]);
        print("Recv handout id: %i\n", msg_p->buff[0]);
        break;
    }
    case CLAIMCONDUCTORID ... CLAIMCONDUCTORID + MAX_BOARDS - 1: {
        if (self->m_board_handler_p->node_states[RANK] == CONDUCTOR)
        {
            ASYNC(self->m_board_handler_p, handle_conductorship_request, msg_p->msgId - CLAIMCONDUCTORID);
        }
        else
        {
            ASYNC(self->m_board_handler_p, lowest_request_index, msg_p->msgId - CLAIMCONDUCTORID);
        }
        break;
    }
    default:
        break;
    }
}
